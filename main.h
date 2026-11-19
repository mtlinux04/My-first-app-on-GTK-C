typedef struct {
    GtkEntry *entry;
    GtkListBox *listbox;
    GtkWidget *dialog;
    GtkListBoxRow *row_to_delete;
} TaskData;

typedef struct {
    GtkListBox *search_listbox;
} SearchData;

void on_dialog_destroy(GtkWidget *widget, gpointer user_data);
void on_tick_button_active(GObject *object, gpointer user_data);
void on_entry_button_active(GtkEntry *entry, gpointer user_data);
void rename_task(GtkEditable *editable, gpointer user_data);
void rename_task_dialog(GSimpleAction *action, GVariant *variant, gpointer user_data);
void complete_task(GObject *object, gpointer user_data);
gboolean filter_list(GtkListBoxRow *row, gpointer user_data);
void on_search_entry(GtkEditable *editable, gpointer user_data);
void search_dialog(GtkButton *btn, gpointer user_data);
void delete_task_confirmed(GtkButton *btn, gpointer user_data);
void delete_task_cancelled(GtkButton *btn, gpointer user_data);
void delete_task_dialog(GSimpleAction *action, GVariant *parameter, gpointer user_data);
void on_menu_button_active(GObject *object, GParamSpec *pspec, gpointer user_data);
void add_task(GtkButton *btn, gpointer user_data);
void add_task_dialog(GtkButton *btn, gpointer user_data);
void app_activate(GApplication *app, gpointer *user_data);
