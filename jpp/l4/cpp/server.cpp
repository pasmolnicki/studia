#include <algorithm>
#include <chrono>
#include <condition_variable>
#include <cstddef>
#include <format>
#include <iostream>
#include <memory>
#include <mutex>
#include <optional>
#include <random>
#include <ranges>
#include <string>
#include <thread>
#include <vector>

enum class SendState {
  Idle,
  Requested,
  Accepcted,
  WaitingForDelivery,
  Completed
};

using Message = struct Message {
  int receiver{0};
  int sender{0};
  std::string message;

  Message(int receiver, int sender, std::string msg)
      : receiver(receiver), sender(sender), message(msg) {}

  Message(const Message &other)
      : receiver(other.receiver), sender(other.sender), message(other.message) {
  }

  Message &operator=(const Message &other) {
    receiver = other.receiver;
    sender = other.sender;
    message = other.message;
    return *this;
  }
};

using User = struct User {
  bool terminate{false};
  SendState state{SendState::Idle};
  std::optional<Message> incoming;
  std::optional<Message> outgoing;
  std::mutex mtx{};
  std::condition_variable cv{};
  int id{0};
  int received_messages{0};
  int sent_messages{0};

  User() = default;
  User(int id) : id(id) {}

  User(const User &u)
      : terminate(u.terminate), state(u.state), incoming(u.incoming),
        outgoing(u.outgoing), id(u.id), received_messages(u.received_messages),
        sent_messages(u.sent_messages) {}

  User(User &&u)
      : terminate(u.terminate), state(u.state), incoming(std::move(u.incoming)),
        outgoing(std::move(u.outgoing)), id(u.id),
        received_messages(u.received_messages), sent_messages(u.sent_messages) {
  }
};

std::mutex print_mutex;
static std::vector<bool> flag_all_finished{};
static std::mutex mu_finished{};

// Check if all users finished
bool is_finished() {
  std::lock_guard<std::mutex> lock(mu_finished);
  return std::find(flag_all_finished.begin(), flag_all_finished.end(), false) ==
         flag_all_finished.end();
}

// Tag ith user finished
void tag_finished(std::uint32_t i) {
  std::lock_guard<std::mutex> lock(mu_finished);
  flag_all_finished[i] = true;
}

template <typename T>
[[nodiscard("Use the result of random")]]
T get_random(int min, int max) {
  std::mt19937 engine(std::random_device{}());
  return T(std::uniform_int_distribution(min, max)(engine));
}

int get_random_receiver(int n_users) { return get_random<int>(0, n_users - 1); }

inline void sync_print(std::string msg) {
  std::lock_guard<std::mutex> lock(print_mutex);
  std::cout << msg << '\n';
  std::cout.flush();
}

void request_send(User &user, int n_messages) {
  std::lock_guard<std::mutex> lock(user.mtx);

  if (user.state != SendState::Idle) {
    return;
  }

  if (user.sent_messages < n_messages) {
    user.state = SendState::Requested;
  }
}

void user(User &user, int n_users, int n_messages) {
  bool untagged = true;
  while (true) {
    request_send(user, n_messages);
    std::unique_lock<std::mutex> lock(user.mtx);

    user.cv.wait(lock, [&user]() {
      return user.state == SendState::Accepcted || user.incoming.has_value() ||
             user.state == SendState::Completed || user.terminate;
    });

    if (user.terminate) {
      break;
    }

    if (user.incoming) {
      auto msg = user.incoming.value();
      user.incoming.reset();
      user.received_messages++;

      sync_print("\tUser[" + std::to_string(user.id) + "] got message: \"" +
                 msg.message + "\" from: " + std::to_string(msg.sender));
    }

    if (user.state == SendState::Accepcted) {
      // Server accepted previous request
      // create the message
      auto receiver = get_random_receiver(n_users);
      user.outgoing = {Message(receiver, user.id, std::string("Hello"))};

      user.state = SendState::WaitingForDelivery;
      lock.unlock();
      user.cv.notify_one(); // notify server
      lock.lock();
    }

    if (user.state == SendState::Completed) {
      user.state = SendState::Idle;
      user.sent_messages++;

      if (untagged && user.sent_messages >= n_messages) {
        tag_finished(user.id);
        untagged = true;
      }

      lock.unlock();

      sync_print("\tUser[" + std::to_string(user.id) + "] send completed");
    }
  }
}

void terminate_users(std::vector<User> &users) {
  for (auto &user : users) {
    std::unique_lock<std::mutex> lock(user.mtx);
    user.terminate = true;
    lock.unlock();
    user.cv.notify_one();
    sync_print(std::format("[server] Terminating: {}", user.id));
  }
}

// Can start the iteration from last end -> save the iter
std::pair<User *, int> get_waiting_sender(std::vector<User> &users,
                                          int last_iter) {
  for (std::size_t c = 0; c < users.size(); c++, last_iter++) {
    if (last_iter == users.size()) {
      last_iter = 0;
    }

    auto &user = users[last_iter];
    std::unique_lock<std::mutex> lock(user.mtx);
    if (user.state == SendState::Requested) {
      user.state = SendState::Accepcted;
      return {&user, last_iter};
    }
  }

  return {nullptr, last_iter};
}

void accept(User &user) {
  {
    std::lock_guard<std::mutex> lock(user.mtx);
    user.state = SendState::Accepcted;
  }
  user.cv.notify_one();
}

auto wait_for_message(User &user) {
  std::unique_lock<std::mutex> lock(user.mtx);

  user.cv.wait(lock, [&]() { return user.outgoing.has_value(); });

  auto msg = user.outgoing.value();
  user.outgoing.reset();
  return msg;
}

void deliver(User &receiver, const Message &msg) {
  {
    std::lock_guard<std::mutex> lock(receiver.mtx);
    receiver.incoming = msg;
  }
  receiver.cv.notify_one();
}

void complete_send(User &user) {
  {
    std::lock_guard<std::mutex> lock(user.mtx);
    user.state = SendState::Completed;
  }
  user.cv.notify_one();
}

void server(std::vector<User> &users) {
  int last_iter = 0;
  while (true) {
    auto [waiting_sender, iter] = get_waiting_sender(users, last_iter);
    last_iter = iter;
    if (waiting_sender == nullptr) {
      if (is_finished()) {
        terminate_users(users);
        break;
      }
      std::this_thread::yield();
      continue;
    }

    sync_print(std::format("[server] Got user: {}", waiting_sender->id));
    auto &user = *waiting_sender;
    accept(user);
    sync_print(std::format("[server] Accepcted user: {}", user.id));
    auto message = wait_for_message(user);
    sync_print(std::format("[server] Forwarding message: {} to {}",
                           message.message, message.receiver));
    deliver(users[message.receiver], message);
    sync_print(std::format("[server] Delivered message"));
    complete_send(user);
  }

  sync_print("[server] Finished...");
}

int main(int argc, char **argv) {
  if (argc != 3) {
    std::cout << "Usage: " << argv[0] << " <n_users> <n_messages_per_user>\n";
    std::cout << "Example: " << argv[0] << " 5 10\n";
    return -1;
  }

  auto n_users = std::atoi(argv[1]);
  auto n_messages = std::atoi(argv[2]);

  flag_all_finished.resize(n_users, false);
  std::vector<User> users{};
  users.reserve(n_users);

  std::vector<std::thread> user_threads{};
  user_threads.reserve(n_users);

  for (int i = 0; i < n_users; i++) {
    users.push_back(User(i));
    user_threads.emplace_back(user, std::ref(users.back()), n_users,
                              n_messages);
  }

  std::thread server_thread(server, std::ref(users));

  for (auto &thread : user_threads) {
    if (thread.joinable()) {
      thread.join();
    }
  }

  if (server_thread.joinable()) {
    server_thread.join();
  }

  // Print summary
  auto received_view = users | std::views::transform([](const User &s) {
                         return s.received_messages;
                       });

  auto sent_view = users | std::views::transform(
                               [](const User &s) { return s.sent_messages; });
  auto received_total =
      std::accumulate(received_view.begin(), received_view.end(), 0);
  auto sent_total = std::accumulate(sent_view.begin(), sent_view.end(), 0);
  auto [min_it, max_it] =
      std::minmax_element(received_view.begin(), received_view.end());
  auto [min, max] = std::make_pair(*min_it, *max_it);

  std::cout << "\n---------- Results ----------\n";
  std::cout << std::format("Total sent: {}\n", sent_total);
  std::cout << std::format("Total received: {}\n", received_total);
  std::cout << std::format("Min-max received messages: min={} max={}\n", min,
                           max);

  for (auto i = 0; i < n_users; i++) {
    auto &u = users[i];
    std::cout << std::format("\t[{}] received: {}, sent: {}\n", i,
                             u.received_messages, u.sent_messages);
  }
}
