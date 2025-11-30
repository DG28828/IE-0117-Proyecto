#include <gtk/gtk.h>
#include <gtk/gtkx.h>

typedef struct {
    int Entero;                              // Para pasar enteros
    char * path;                             // Para pasar path de archivo actual
    GtkWidget * dataimport_picker;           // Para pasar widget de picker
    GtkWidget * socket;                      // Para pasar widget de socket (contenedor de grafica)
} Datos_Callback;

static void importar (GtkWidget *widget, gpointer user_data) {
    
    Datos_Callback * Datos = (Datos_Callback*) user_data; //Cast del gpointer 
    GtkWidget *dataimport_picker = Datos -> dataimport_picker;
    
    GtkFileChooser *chooser = GTK_FILE_CHOOSER(dataimport_picker);
    GFile *archivo = gtk_file_chooser_get_file(chooser);   // Extraer identificador GFile del archivo seleccionado 
    if (archivo != NULL) {                                   // Verificar que *archivo no es null
	gchar *path = g_file_get_path(archivo);            // Extraer path del archivo

	if (path != NULL) {
	    g_print("Archivo seleccionado: %s\n", path);

	    if (Datos -> path != NULL) {   //Liberar espacio si estuviera ocupado
                g_free(Datos -> path);
            }

	    Datos -> path = g_strdup(path);   //Copia del string con path a struct
	    
	    g_free(path);                                  // Liberar el string (según documentación)
	}
	g_object_unref(archivo);                           // Liberar el archivo (según documentación)
    }
    else {
	g_print("No se seleccionó ningún archivo\n");
    }
}

static void visualizar (GtkWidget *widget, gpointer user_data) {
    Datos_Callback * Datos = (Datos_Callback*) user_data; //Cast del gpointer
    GtkWidget *socket = Datos -> socket;
    char * file_path = Datos -> path;
    
    //GdkWindow *ventana_gdk = gtk_widget_get_window(socket);
    guint32 ID = gtk_socket_get_id(GTK_SOCKET(socket));
    g_print("Socket XID: %#x\n", ID);

    if (file_path != NULL) {
	g_print("Visualizando archivo: %s\n", file_path);

	FILE * fp = popen("gnuplot -persist", "w");  // w: write
	if (fp == NULL) {
	    g_print("Error: no fue posible ejecutar gnuplot\n");
	    return;
	}
	fprintf(fp, "set terminal x11\n");
	//fprintf(fp, "set output\n");
	fprintf(fp, "set term x11 window '%u'\n", ID);
	
	fprintf(fp, "set title 'Serie temporal'\n");
	fprintf(fp, "set xlabel 'Tiempo'\n");
	fprintf(fp, "set ylabel 'Magnitud'\n");
	fprintf(fp, "set datafile separator ';'\n");
	fprintf(fp, "plot '%s' using 1:2 with lines title 'Señal'\n", file_path);
	fprintf(fp, "pause mouse\n");
	
	pclose(fp);
    }
    else {
	g_print("Error: Importe primero un archivo\n");
    }

    
}

static void activate (GtkApplication* app, gpointer user_data) {

    // ***************  DECLARAR Variables ***************
    
    GtkWidget *window;              // Ventana principal
    GtkWidget *window_grid;         // Grid (cuadricula base para contener otros widgets)
    //GtkWidget *plot_area;           // Area para colocar la grafica
    GtkWidget *socket;              // Socket para grafica
    GtkWidget *plot_frame;          // Marco para el area de la grafica
    GtkWidget *dataimport_grid;     // Grid para area de importacion de datos
    GtkWidget *dataimport_frame;    // Marco para area de importacion de datos
    GtkWidget *dataimport_label;    // Etiqueta para importacion de datos
    GtkWidget *dataimport_picker;   // Seleccionador para importacion de datos
    GtkWidget *dataimport_button;   // Botón para importar datos
    GtkWidget *boton_visualizar;    // Boton para visualizacion de datos

    static Datos_Callback Datos;    // Struct para pasar información a funciones de callbacks
    
    // ***************  VENTANA PRINCIPAL ***************
    
    // Crear ventana principal
    window = gtk_application_window_new (app);                             // Ventana principal
    gtk_window_set_title (GTK_WINDOW (window), "Visualizador de datos");   // Titulo

    //Propiedades de la ventana
    gtk_container_set_border_width (GTK_CONTAINER (window), 25);           // Borde
    //gtk_window_set_default_size (GTK_WINDOW (window), 1200, 800);         // Tamaño de ventana
    //gtk_window_maximize(GTK_WINDOW (window));                             // Maximizar

    
    // ***************   GRID PRINCIPAL   ***************
    
    // Crear grid y definir cantidad de filas y columnas
    window_grid = gtk_grid_new ();
    for (int i = 0; i<2; i++) {                                            // Filas del grid
	gtk_grid_insert_row (GTK_GRID(window_grid), i);
    }
    for (int i = 0; i<4; i++) {                                            // Columnas del grid
	gtk_grid_insert_column (GTK_GRID(window_grid), i);
    }

    // Propiedades del grid
    gtk_grid_set_row_spacing (GTK_GRID (window_grid), 25);                 // Espaciamiento de filas
    gtk_grid_set_column_spacing (GTK_GRID (window_grid), 25);              // Espaciamiento de columnas
    gtk_grid_set_row_homogeneous(GTK_GRID(window_grid), FALSE);            // Filas homogeneas: Falso
    gtk_grid_set_column_homogeneous(GTK_GRID(window_grid), TRUE);          // Columnas homogeneas: Verdadero

    // Contener
    gtk_container_add (GTK_CONTAINER (window), window_grid);               // Contener el grid en window

    // (AUXILIAR, etiqueta al grid para visualizarlo)
    for (int i = 0; i < 2; i++) {
	for (int j = 0; j < 4; j++) {
	    GtkWidget *label = gtk_label_new("");
	    gtk_grid_attach(GTK_GRID(window_grid), label, j, i, 1, 1);
	}
    }
    
    // ********** CONTENEDOR PARA IMPORTACION DE DATOS *************
    
    // Crear grid
    dataimport_grid = gtk_grid_new ();

    // Propiedades del grid
    gtk_grid_set_row_spacing (GTK_GRID (dataimport_grid), 10);              // Espaciamiento de filas
    gtk_grid_set_column_spacing (GTK_GRID (dataimport_grid), 10);           // Espaciamiento de columnas
    gtk_grid_set_column_homogeneous(GTK_GRID(dataimport_grid), FALSE);      // Filas homogeneas: Falso
    gtk_grid_set_column_homogeneous(GTK_GRID(dataimport_grid), FALSE);      // Columnas homogeneas: Falso
    gtk_container_set_border_width (GTK_CONTAINER (dataimport_grid), 10);   // Borde

    // Crear frame
    dataimport_frame = gtk_frame_new("Importación de datos");

    // Contener
    gtk_container_add(GTK_CONTAINER(dataimport_frame), dataimport_grid);    // Contener grid en el frame
    gtk_grid_attach(GTK_GRID(window_grid), dataimport_frame, 0, 0, 1, 1);   //Contener frame en el grid principal

    // (AUXILIAR, etiqueta al grid para visualizarlo)
    for (int i = 0; i < 1; i++) {
	for (int j = 0; j < 3; j++) {
	    GtkWidget *label = gtk_label_new("");
	    gtk_grid_attach(GTK_GRID(dataimport_grid), label, j, i, 1, 1);
	}
    }

    
    // *************** SELECCIONADOR DE ARCHIVOS ***************
    
    // Crear etiqueta
    dataimport_label = gtk_label_new("Archivo:");                           // Nombre de la etiqueta
    gtk_widget_set_halign (dataimport_label, GTK_ALIGN_START);              // ALineacion horizontal
    gtk_widget_set_valign (dataimport_label, GTK_ALIGN_CENTER);             // Alineacion vertical
    gtk_widget_set_hexpand (dataimport_label, FALSE);                       // Expandir: Falso

    // Crear seleccionador
    dataimport_picker = gtk_file_chooser_button_new ("Seleccione un archivo", GTK_FILE_CHOOSER_ACTION_OPEN);
    gtk_file_chooser_set_local_only(GTK_FILE_CHOOSER(dataimport_picker), FALSE);
    gtk_widget_set_hexpand (dataimport_picker, TRUE);                       // Expandir: Verdadero

    // Contener
    gtk_grid_attach(GTK_GRID(dataimport_grid), dataimport_label, 0, 0, 1, 1); // Contener label en grid imp
    gtk_grid_attach(GTK_GRID(dataimport_grid), dataimport_picker, 1, 0, 1, 1);// COntener picker en grid imp

    
    // ***************   BOTON IMPORTAR   ***************

    
    // Crear boton
    dataimport_button = gtk_button_new_with_label ("Importar");

    // Datos a pasar al callback
    Datos.path = NULL;                   // Para obtener path
    Datos.dataimport_picker = dataimport_picker;      // Para pasar Widget 
  
    g_signal_connect (dataimport_button, "clicked", G_CALLBACK (importar), &Datos); // Conectar señal
    gtk_grid_attach(GTK_GRID(dataimport_grid), dataimport_button, 2, 0, 1, 1); // Contener

    
    // ***************     CONTENEDOR PARA GRAFICAR      ***************

    // Crear socket
    socket = gtk_socket_new();

    // Propiedades
    gtk_widget_set_size_request(socket, 1500, 600);                        // Tamaño

    // Mostrar socket
    gtk_widget_show(socket);

    // Crear frame
    plot_frame = gtk_frame_new("Gráfica");
    
    // Contener
    gtk_container_add(GTK_CONTAINER(plot_frame), socket);                     // Contener socket en el frame
    gtk_grid_attach(GTK_GRID(window_grid), plot_frame, 0, 1, 4, 1);           // Contener el frame en el grid

    
    // *************** BOTON VISUALIZACION ***************
    
    // Crear boton
    boton_visualizar = gtk_button_new_with_label ("Visualizar");

    // Datos a pasar el callback
    Datos.socket = socket;                         // Pasar Widget
    
    g_signal_connect (boton_visualizar, "clicked", G_CALLBACK (visualizar), &Datos);
    gtk_grid_attach(GTK_GRID(window_grid), boton_visualizar, 3, 0, 1, 1);

    
    // ***************  MOSTRAR RESULTADO  ***************
    gtk_widget_show_all (window);
}

int main (int argc, char **argv) {
    GtkApplication *app;
    int status;

    app = gtk_application_new ("org.gtk.DataVisualization", G_APPLICATION_FLAGS_NONE);
    g_signal_connect (app, "activate", G_CALLBACK (activate), NULL);
    status = g_application_run (G_APPLICATION (app), argc, argv);
    g_object_unref (app);

    return status;
}
