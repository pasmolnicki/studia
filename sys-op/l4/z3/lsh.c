#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdbool.h>
#include <stdatomic.h>
#include <termios.h>
#include <fcntl.h>

#define N_HISTORY 10
#define MAX_ARGS 64
#define MAX_COMMAND_LENGTH 256
#define MAX_PATH 1024


typedef struct Command {
    char** args;
    int length;
    pid_t pid;
    struct Command* next;
} Command;

typedef struct CommandList {
    Command* head;
    Command* tail;
    int length;
    bool is_background;
    char* input;
} CommandList;

static atomic_char background_task_count = ATOMIC_VAR_INIT(0);
static char history[N_HISTORY][MAX_COMMAND_LENGTH];
static int history_count = 0;

void safe_strcpy(char* dest, const char* src) {
    (void)strncpy(dest, src, MAX_COMMAND_LENGTH - 1);
    dest[MAX_COMMAND_LENGTH - 1] = '\0';
}

CommandList* create_command_list() {
    CommandList* list = (CommandList*)calloc(1, sizeof(CommandList));
    list->head = NULL;
    list->tail = NULL;
    list->length = 0;
    list->is_background = false;
    list->input = NULL;
    return list;
}

void delete_command_list(CommandList* list) {
    Command* current = list->head;
    while (current != NULL) {
        Command* next = current->next;
        free(current->args);
        free(current);
        current = next;
    }
    free(list);
}

void print_command_list(CommandList* list) {
    Command* current = list->head;
    if (current == NULL) {
        printf("Command list is empty.\n");
        return;
    }
    if (list->is_background) {
        printf("bg-mode\n");
    }
    while (current != NULL) {
        for (int i = 0; i < current->length; i++) {
            printf("%s ", current->args[i]);
        }
        printf("\n");
        current = current->next;
    }
}

void add_command(CommandList* list, char** args) {
    Command* cmd = (Command*)calloc(1, sizeof(Command));
    cmd->length = 0;
    list->length++;

    // Deep copy args
    while (args[cmd->length] != NULL) {
        cmd->length++;
    }
    cmd->args = (char**)calloc(cmd->length + 1, sizeof(char*));
    for (int i = 0; i < cmd->length; i++) {
        cmd->args[i] = strdup(args[i]);
    }
    cmd->args[cmd->length] = NULL;
    cmd->next = NULL;

    if (list->head == NULL) {
        list->head = cmd;
        list->tail = cmd;
    } else {
        list->tail->next = cmd;
        list->tail = cmd;
    }
}

CommandList* parse_command(const char* line) {
    static char* args[MAX_ARGS];
    char* token;
    char* command_copy = strdup(line);
    int index = 0;
    CommandList* list = create_command_list();
    list->input = strdup(line);

    token = strtok(command_copy, " \n");
    while (token != NULL && index < MAX_ARGS - 1) {
        // Check if token is an environment variable
        if (token[0] == '$') {
            char* env_var = getenv(token + 1);
            if (env_var != NULL) {
                args[index++] = env_var;
            } else {
                args[index++] = token; // Keep original token if env var not found
            }
        } else if (strcmp(token, "|") == 0) {
            args[index] = NULL; // Null-terminate current args
            if (args[0] != NULL) {
                add_command(list, args);
            }
            index = 0; // Reset index for next command
        }
        else {
            args[index++] = token;
        }

        token = strtok(NULL, " \n");
    }
    args[index] = NULL;

    // Add the last command if any
    if (index > 0) {
        if (strcmp(args[index - 1], "&") == 0) {
            args[index - 1] = NULL; // Remove '&' from arguments
            list->is_background = true;
            atomic_fetch_add(&background_task_count, 1);
        }

        if (args[0] != NULL) {
            add_command(list, args);
        }
    }

    return list;
}

void handle_history_keys(char arrow, char* line, int* history_index) {
    if (arrow == 'A') { // Up arrow
        if (history_count > 0) {
            for (size_t i = 0; i < strlen(line); i++) {
                printf("\b \b");
            }
            safe_strcpy(line, history[*history_index % N_HISTORY]);
            printf("%s", line);
            if (*history_index > 0) {
                (*history_index)--;
            }
        }
    }
    if (arrow == 'B') { // Down arrow
        if (history_count > 0 && *history_index < history_count - 1) {
            (*history_index)++;
            for (size_t i = 0; i < strlen(line); i++) {
                printf("\b \b");
            }
            safe_strcpy(line, history[*history_index % N_HISTORY]);
            printf("%s", line);
        } else {

            for (size_t i = 0; i < strlen(line); i++) {
                printf("\b \b");
            }
            line[0] = '\0'; // Clear line
        }
    }
}

void handle_escape_char(char* line, int* history_index) {
    // Handle arrow keys for history navigation
    getchar(); // Skip the '[' character
    char arrow = getchar();
    handle_history_keys(arrow, line, history_index);
}

char* get_input_line() {
    static char line[256];
    memset(line, 0, sizeof(line));
    int history_index = history_count - 1;
    
    // Read the input line + catch EOF, arrow keys etc.
    while(1) {
        char ch = getchar();
        if (ch == EOF) {
            return NULL; // EOF detected
        } else if (ch == '\n') {
            line[strlen(line)] = '\0';
            printf("\n");
            break; // End of line
        } else if (ch == 27) { // Escape character
            handle_escape_char(line, &history_index);
        } else if (ch == 127 || ch == 8) { // Backspace
            if (strlen(line) > 0) {
                line[strlen(line) - 1] = '\0';
                printf("\b \b"); // Move cursor back, print space, move back again
            }
        } else {
            line[strlen(line)] = ch;
            printf("%c", ch); // Echo the character
        }
    }


    // Ignore empty lines
    if (strlen(line) == 0) {
        return line;
    }

    // Store in history
    if (history_count < N_HISTORY) {
        strcpy(history[history_count++], line);
    } else {
        for (int i = 1; i < N_HISTORY; i++) {
            strcpy(history[i - 1], history[i]);
        }
        strcpy(history[N_HISTORY - 1], line);
    }

    return line;
}

int exec_cd(char *arg) {
    static char lastdir[MAX_PATH] = "";
    char currentdir[MAX_PATH] = "";

    if (getcwd(currentdir, sizeof(currentdir)) == NULL) {
        perror("getcwd failed");
        return -1;
    }

    if (arg == NULL || strcmp(arg, "~") == 0) {
        arg = getenv("HOME");
    } else if (strcmp(arg, "-") == 0) {
        if (lastdir[0] == '\0') {
            fprintf(stderr, "lsh: cd: OLDPWD not set\n");
            return -1;
        }
        arg = lastdir;
        printf("%s\n", arg);
    }

    if (chdir(arg) != 0) {
        perror("lsh: cd failed");
        return -1;
    }

    strncpy(lastdir, currentdir, MAX_PATH);
    return 0;
}

void zombie_handler(int signo) {
    if (atomic_load(&background_task_count) == 0) {
        return; // Ignore if there are background tasks
    }

    atomic_fetch_sub(&background_task_count, 1);
    while (waitpid(-1, NULL, WNOHANG) > 0);
}

bool is_builtin_command(char** args) {
    return (strcmp(args[0], "cd") == 0 || strcmp(args[0], "history") == 0);
}

void execute_builtin_command(char** args) {
    // Build-in commands
    if (strcmp(args[0], "cd") == 0) {
        exec_cd(args[1]);
    }
    if (strcmp(args[0], "history") == 0) {
        int start = history_count > N_HISTORY ? history_count - N_HISTORY : 0;
        for (int i = start; i < history_count; i++) {
            printf("%d: %s\n", i + 1, history[i % N_HISTORY]);
        }
    }
}

void execute_commands(CommandList* list) {
    // print_command_list(list);
    Command* current = list->head;
    int pipefds[2];
    int prev_read_fd = -1;

    while (current != NULL) {
        bool is_last = (current->next == NULL);

        // Create a pipe if not the last command
        if (!is_last) {
            if (pipe(pipefds) == -1) {
                perror("lsh: pipe failed");
                exit(EXIT_FAILURE);
            }
        }

        // Fork a child process and set up pipes
        pid_t pid = fork();
        if (pid == 0) {
            signal(SIGINT, SIG_DFL); // Restore default Ctrl+C behavior in child

            if (prev_read_fd != -1) {
                if (dup2(prev_read_fd, STDIN_FILENO) == -1) {
                    perror("lsh: dup2 failed");
                    exit(EXIT_FAILURE);
                }
                close(prev_read_fd);
            }

            if (!is_last) {
                if (dup2(pipefds[1], STDOUT_FILENO) == -1) {
                    perror("lsh: dup2 failed");
                    exit(EXIT_FAILURE);
                }
                close(pipefds[0]);
                close(pipefds[1]);
            }


            // Handle < > redirection
            for (int i = 0; i < current->length; i++) {
                if (strcmp(current->args[i], "<") == 0) {
                    if (current->args[i + 1] == NULL) {
                        fprintf(stderr, "lsh: syntax error near unexpected token `newline'\n");
                        exit(EXIT_FAILURE);
                    }

                    // https://pubs.opengroup.org/onlinepubs/7908799/xsh/open.html
                    int fd = open(current->args[i + 1], O_RDONLY | O_CREAT | O_TRUNC, 0644);
                    if (fd == -1) {
                        perror("lsh: open for input redirection failed");
                        exit(EXIT_FAILURE);
                    }
                    if (dup2(fd, STDIN_FILENO) == -1) {
                        perror("lsh: dup2 for input redirection failed");
                        exit(EXIT_FAILURE);
                    }
                    close(fd);
                    current->args[i] = NULL; // Terminate args before <
                } else if (strcmp(current->args[i], ">") == 0) {
                    if (current->args[i + 1] == NULL) {
                        fprintf(stderr, "lsh: syntax error near unexpected token `newline'\n");
                        exit(EXIT_FAILURE);
                    }

                    int fd = open(current->args[i + 1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
                    if (fd == -1) {
                        perror("lsh: open for output redirection failed");
                        exit(EXIT_FAILURE);
                    }
                    if (dup2(fd, STDOUT_FILENO) == -1) {
                        perror("lsh: dup2 for output redirection failed");
                        exit(EXIT_FAILURE);
                    }
                    close(fd);
                    current->args[i] = NULL; // Terminate args before >
                } else if (strcmp(current->args[i], "2>") == 0) {
                    if (current->args[i + 1] == NULL) {
                        fprintf(stderr, "lsh: syntax error near unexpected token `newline'\n");
                        exit(EXIT_FAILURE);
                    }

                    int fd = open(current->args[i + 1], O_WRONLY | O_CREAT | O_APPEND, 0644);
                    if (fd == -1) {
                        perror("lsh: open for output append redirection failed");
                        exit(EXIT_FAILURE);
                    }
                    if (dup2(fd, STDERR_FILENO) == -1) {
                        perror("lsh: dup2 for output append redirection failed");
                        exit(EXIT_FAILURE);
                    }
                    close(fd);
                    current->args[i] = NULL; // Terminate args before >>
                }
            }

            execvp(current->args[0], current->args);

            // This will only executed if exec fails
            if (errno == ENOENT) {
                fprintf(stderr, "lsh: command not found: %s\n", current->args[0]);
            } else {
                fprintf(stderr, "lsh: exec error: %s\n", strerror(errno));
            }

            exit(EXIT_FAILURE);
        } else if (pid < 0) {
            perror("lsh: fork failed\n");
            return;
        }

        current->pid = pid;
        if (prev_read_fd != -1) {
            close(prev_read_fd);
        }

        if (!is_last) {
            close(pipefds[1]); // Close unused write end
            prev_read_fd = pipefds[0];
        }

        current = current->next;
    }

    if (list->is_background) {
        printf("[%d] %s\n", list->head->pid, list->input);
    } else {
        // Wait for all child processes to finish
        while (wait(NULL) > 0);
    }
}

int main() {
    // Don't allow Ctrl+C to terminate the shell
    signal(SIGINT, SIG_IGN);
    signal(SIGCHLD, zombie_handler);

    // Disable terminal buffering for immediate input processing
    struct termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    while (1) {
        printf("lsh> ");
        char* command = get_input_line();
        if (command == NULL) {
            break; // EOF
        }

        if (strcmp(command, "exit") == 0) {
            break;
        }

        CommandList* list = parse_command(command);
        if (list->length == 0) {
            delete_command_list(list);
            continue; // Empty command
        }

        if (list->length == 1 && is_builtin_command(list->head->args)) {
            execute_builtin_command(list->head->args);
            delete_command_list(list);
            continue;
        }

        execute_commands(list);
        delete_command_list(list);
    }

    // Restore terminal settings
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return 0;
}