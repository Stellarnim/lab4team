#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define BROADCAST_ADDRESS "127.0.0.255"  // 브로드캐스트 주소
#define PORT 5000
#define BUFFER_SIZE 1024

GtkBuilder *builder;

GtkWidget *main_window;
GtkWidget *chat_window;

const gchar *username ='\0';
const gchar *msg = '\0';

int sockfd;
struct sockaddr_in broadcast_addr;
char buffer[BUFFER_SIZE];
int broadcast_enable = 1;
void broadcast_client();
void broadcast_recieve();
void append_text_to_textview(GtkWidget *textview, const gchar *text);
void update_display();
void on_submit_clicked(GtkButton *button, gpointer user_data);
void on_send_clicked(GtkButton *button, gpointer user_data);

void broadcast_client(){
    // 소켓 생성
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // 소켓 구성
    memset(&broadcast_addr, 0, sizeof(broadcast_addr));
    broadcast_addr.sin_family = AF_INET;
    broadcast_addr.sin_addr.s_addr = inet_addr(BROADCAST_ADDRESS);
    broadcast_addr.sin_port = htons(PORT);

    // 서버 소켓에 연결
    if (connect(sockfd, (struct sockaddr *)&broadcast_addr, sizeof(broadcast_addr)) < 0) {
        perror("Connection failed");
        exit(EXIT_FAILURE);
    }    
}

void broadcast_recieve(){
    GtkWidget *textview = GTK_WIDGET(gtk_builder_get_object(builder, "chat_display"));
    memset(buffer, 0, sizeof(buffer));

    // Receive the broadcasted message
    recv(sockfd, buffer, sizeof(buffer), 0);
    append_text_to_textview(textview, buffer);
}

void update_display() {
   
}

void append_text_to_textview(GtkWidget *textview_widget, const gchar *text) {
    GtkTextBuffer *buffer;
    GtkTextIter iter;

    // 위젯이 GtkTextView인지 확인
    if (GTK_IS_TEXT_VIEW(textview_widget)) {
        // GtkTextView에서 버퍼를 가져옴
        buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(textview_widget));

        // 버퍼의 끝으로 이동
        gtk_text_buffer_get_end_iter(buffer, &iter);

        // 텍스트를 버퍼에 삽입
        gtk_text_buffer_insert(buffer, &iter, text, -1);
    } else {
        g_print("오류: 제공된 위젯이 GtkTextView가 아닙니다.\n");
    }
}

void on_submit_clicked(GtkButton *button, gpointer user_data) {
    GtkWidget *chatname = GTK_WIDGET(gtk_builder_get_object(builder, "chatname"));
    GtkWidget *usname = GTK_WIDGET(gtk_builder_get_object(builder, "usname"));

    const gchar *chatid = gtk_entry_get_text(GTK_ENTRY(chatname));
    username = g_strdup(chatid);

    gtk_entry_set_text(GTK_ENTRY(usname), username);
    gtk_widget_show_all(chat_window);
    gtk_widget_hide(main_window);
    broadcast_client();
    update_display();
}

void on_send_clicked(GtkButton *button, gpointer user_data) {
    GtkWidget *sender = GTK_WIDGET(gtk_builder_get_object(builder, "sender"));

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

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);

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