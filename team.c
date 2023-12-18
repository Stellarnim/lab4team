#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define BROADCAST_ADDRESS "127.0.0.1"  // 브로드캐스트 주소
#define PORT 5000
#define BUFFER_SIZE 1024

GtkBuilder *builder;

GtkWidget *main_window;
GtkWidget *chat_window;

const gchar *username ='\0';
const gchar *msg = '\0';



void update_display() {
   
}

void on_submit_clicked(GtkButton *button, gpointer user_data) {
    GtkWidget *chatname = GTK_WIDGET(gtk_builder_get_object(builder, "chatname"));
    GtkWidget *usname = GTK_WIDGET(gtk_builder_get_object(builder, "usname"));

    const gchar *chatid = gtk_entry_get_text(GTK_ENTRY(chatname));
    username = g_strdup(chatid);

    gtk_entry_set_text(GTK_ENTRY(usname), username);
    gtk_widget_show_all(chat_window);
    gtk_widget_hide(main_window);

    update_display();
}

void on_send_clicked(GtkButton *button, gpointer user_data) {
    GtkWidget *sender = GTK_WIDGET(gtk_builder_get_object(builder, "sender"));
    GtkWidget *textview = GTK_WIDGET(gtk_builder_get_object(builder, "chat_display"));

    const gchar *text = gtk_entry_get_text(GTK_ENTRY(sender));
    const gchar *userid = username;
    
    char* message = g_strdup_printf("%s : %s\n", userid, text);
    if (sendto(sockfd, message, strlen(message), 0, (struct sockaddr*)&broadcast_addr, sizeof(broadcast_addr)) < 0) {
        perror("sendto");
        exit(EXIT_FAILURE);
    }
    
    broadcast_recieve();

    g_free(message);

    update_display();
}

void broadcast_client(){
    int sockfd;
    struct sockaddr_in broadcast_addr;
    char buffer[BUFFER_SIZE];
    int broadcast_enable = 1;

    // 소켓 생성
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // 브로드캐스트 속성 설정
    if (setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &broadcast_enable, sizeof(broadcast_enable)) < 0) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    // 소켓 구성
    memset(&broadcast_addr, 0, sizeof(broadcast_addr));
    broadcast_addr.sin_family = AF_INET;
    broadcast_addr.sin_addr.s_addr = inet_addr(BROADCAST_ADDRESS);
    broadcast_addr.sin_port = htons(PORT);

    if (connect(sockfd, (struct sockaddr *)&broadcast_address, sizeof(broadcast_address)) < 0) {
        perror("Connection failed");
        exit(EXIT_FAILURE);
    }
}

void broadcast_recieve(){
    memset(buffer, 0, sizeof(buffer));

    // Receive the broadcasted message
    recv(client_socket, buffer, sizeof(buffer), 0);

    append_text_to_textview(textview, buffer);
}

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);
    broadcast_client();

    builder = gtk_builder_new_from_file("Chatprogram.glade");

    main_window = (GtkWidget*)gtk_builder_get_object(builder, "main_window");
    chat_window = (GtkWidget*)gtk_builder_get_object(builder, "chat_window");

    gtk_builder_connect_signals(builder, NULL);
    g_signal_connect(main_window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    g_signal_connect(chat_window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    gtk_widget_show_all(main_window);
    gtk_widget_hide(chat_window);

    gtk_main();

    g_object_unref(builder);
    
    close(sockfd);
    return 0;
}