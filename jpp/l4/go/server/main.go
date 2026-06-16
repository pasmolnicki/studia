package main

import (
	"fmt"
	"math/rand"
	"os"
	"runtime"
	"strconv"
	"sync"
	"sync/atomic"
)

type Message struct {
	msg      string
	sender   int
	receiver int
}

type User struct {
	id           int
	is_intrested chan bool
	can_send     chan bool
	is_delivered chan bool
	outgoing     chan Message
	incoming     chan Message
	terminate    chan bool
	n_sent       int
	n_received   int
	finished     atomic.Bool
}

func NewUser(id int) *User {
	return &User{
		id:           id,
		is_intrested: make(chan bool, 1),
		can_send:     make(chan bool),
		is_delivered: make(chan bool),
		outgoing:     make(chan Message),
		incoming:     make(chan Message),
		terminate:    make(chan bool),
		n_sent:       0,
		n_received:   0,
	}
}

type SyncPrint struct {
	mu sync.Mutex
}

func (sp *SyncPrint) Println(s string) {
	sp.mu.Lock()
	defer sp.mu.Unlock()
	fmt.Printf("%s\n", s)
}

func random_user(n_users int) int {
	return rand.Intn(n_users)
}

func user_loop(user *User, sp *SyncPrint, n_users, n_messages int) {
	untagged := false

	user.is_intrested <- true
	sp.Println(fmt.Sprintf("\tUser[%d] is interestd", user.id))

	for {
		// Wait for approval from server / incoming request
		select {
		case received := <-user.incoming:
			user.n_received++
			sp.Println(
				fmt.Sprintf("\tUser[%d] received: \"%s\" from %d", user.id, received.msg, received.sender))

		case <-user.terminate:
			return

		// Wait until user can send another n_messages
		// only server can let us do it
		case <-user.can_send:
			// Server chose this user to write it's message
			recipent := random_user(n_users)
			for recipent == user.id {
				recipent = random_user(n_users)
			}

			msg := Message{
				msg:      "Hello",
				sender:   user.id,
				receiver: recipent,
			}
			user.outgoing <- msg
			user.n_sent++
			sp.Println(fmt.Sprintf("\tUser[%d] sending message: \"%s\" to: %d", user.id, msg.msg, msg.receiver))

			if user.n_sent >= n_messages && !untagged {
				untagged = true
				user.finished.Store(true)
			} else {
				user.is_intrested <- true
			}

			// Block until the message is delivered
			// otherwise it would be a buffered solution wth buffer size = 1
			<-user.is_delivered
		}

	}
}

func choose_user(users []*User, last_iter int) (*User, int) {
	for range users {
		last_iter++
		if last_iter == len(users) {
			last_iter = 0
		}

		user := users[last_iter]
		select {
		case <-user.is_intrested:
			return user, last_iter
		default:
			continue
		}
	}

	return nil, 0
}

func check_if_all_finished(users []*User) bool {
	for i := range users {
		if !users[i].finished.Load() {
			return false
		}
	}
	return true
}

func clean_up_server(users []*User) {
	for i := range users {
		u := users[i]
		close(u.can_send)
		close(u.is_intrested)
		close(u.is_delivered)
		close(u.incoming)
		close(u.outgoing)
		close(u.terminate)
	}
}

func terminate_all(users []*User, sp *SyncPrint) {
	for i := range users {
		sp.Println(fmt.Sprintf("[server] Terminating: %d...", users[i].id))
		users[i].terminate <- true
	}
}

func server(users []*User, sp *SyncPrint) {
	last_iter := 0
	for {
		sender, end_iter := choose_user(users, last_iter)
		last_iter = end_iter

		if sender == nil {
			if check_if_all_finished(users) {
				break
			}
			runtime.Gosched()
			continue
		}

		// Allow user to make the message
		sp.Println(fmt.Sprintf("[server] Chose user: %d", sender.id))
		sender.can_send <- true
		sp.Println(fmt.Sprintf("[server] Allowed user[%d] to send", sender.id))
		// Get the message
		msg := <-sender.outgoing
		sp.Println(fmt.Sprintf("[server] Forwarding message: \"%s\" to: %d", msg.msg, msg.receiver))
		// Forward the message
		receiver := users[msg.receiver]
		receiver.incoming <- msg
		// Unblock the sender
		sender.is_delivered <- true
		sp.Println("[server] Message delivered")
	}

	terminate_all(users, sp)
	clean_up_server(users)
	sp.Println("[server] Finished")
}

func main() {
	if len(os.Args) != 3 {
		fmt.Printf("Usage: %s <n_users> <n_messages_per_user>\n", os.Args[0])
		fmt.Printf("Example: %s 5 10\n", os.Args[0])
		return
	}

	n_users, err := strconv.Atoi(os.Args[1])
	if err != nil {
		fmt.Println(err.Error())
		return
	}

	n_messages_per_user, err := strconv.Atoi(os.Args[2])
	if err != nil {
		fmt.Println(err.Error())
		return
	}

	sp := &SyncPrint{}
	users := make([]*User, n_users)
	for i := range users {
		users[i] = NewUser(i)
		go user_loop(users[i], sp, n_users, n_messages_per_user)
	}

	server(users, sp)
	fmt.Println("--------- Results ---------")

	min_received := (1 << 31)
	max_received := 0
	total_sent := 0
	total_received := 0
	for _, u := range users {
		total_received += u.n_received
		total_sent += u.n_sent

		if min_received > u.n_received {
			min_received = u.n_received
		}
		if max_received < u.n_received {
			max_received = u.n_received
		}
	}

	fmt.Printf("Total sent: %d\n", total_sent)
	fmt.Printf("Total received: %d\n", total_received)
	fmt.Printf("Min-max received messages: min=%d max=%d\n", min_received, max_received)
	for _, u := range users {
		fmt.Printf("\tUser[%d] received: %d, sent: %d\n", u.id, u.n_received, u.n_sent)
	}
}
