#include <gtk/gtk.h>

GtkBuilder *builder;

GtkWidget *main_window;
GtkWidget *chat_window;

const gchar *username ='\0';

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

    return 0;
}