#include <stdio.h>
#include <gtk/gtk.h>
#include <gtk/gtkx.h>

typedef struct {
    int Entero;                              // Para pasar enteros
    char * path;                             // Para pasar path de archivo actual
    GtkWidget * dataimport_picker;           // Para pasar widget de picker
    GtkWidget * plot_container;              // Para pasar widget de contenedor de grafica
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
    GtkWidget * plot_container = Datos -> plot_container;
    char * file_path = Datos -> path;
    

    if (file_path != NULL) {
	g_print("Visualizando archivo: %s\n", file_path);

	remove("plot_temporal.png");    // Eliminar grafico en caso de existir

	FILE * fp = popen("gnuplot -persist", "w");  // w: write
	if (fp == NULL) {
	    g_print("Error: no fue posible ejecutar gnuplot\n");
	    return;
	}
	fprintf(fp, "set terminal png size 1500,600\n");   // Tamaño del grafico
	fprintf(fp, "set output 'plot_temporal.png'\n");   // PNG de salida
	
	fprintf(fp, "set title 'Serie temporal'\n");
	fprintf(fp, "set xlabel 'Tiempo'\n");
	fprintf(fp, "set ylabel 'Magnitud'\n");
	fprintf(fp, "set datafile separator ' \t,;'\n");
	fprintf(fp, "plot '%s' using 1:2 with lines title 'Señal'\n", file_path);
	fprintf(fp, "exit\n");
	
	pclose(fp);
    }
    else {
	g_print("Error: Importe primero un archivo\n");
    }

    // Limpiar contenedor
    GList * children_list = gtk_container_get_children(GTK_CONTAINER(plot_container)); // Lista de hijos en plot_container
    for (GList * i = children_list; i != NULL; i = i -> next) {
	gtk_container_remove(GTK_CONTAINER(plot_container), GTK_WIDGET(i -> data));
    }
    g_list_free(children_list);   //Liberar memoria de lista de hijos en plot_container

    // Colocar imagen en contenedor plot_container
    GtkWidget * imagen = gtk_image_new_from_file("plot_temporal.png");

    // Contener imagen en plot_container
    gtk_container_add(GTK_CONTAINER(plot_container), imagen);
    gtk_widget_show_all(plot_container);

    remove("plot_temporal.png");   // Eliminar grafica temporal generada
    
}

static void activate (GtkApplication* app, gpointer user_data) {

    // ***************  DECLARAR Variables ***************
    
    GtkWidget *window;              // Ventana principal
    GtkWidget *window_grid;         // Grid (cuadricula base para contener otros widgets)
    GtkWidget *plot_container;      // Contenedor box para grafica
    GtkWidget *plot_frame;          // Marco para el area de la grafica
    GtkWidget *dataimport_grid;     // Grid para area de importacion de datos
    GtkWidget *dataimport_frame;    // Marco para area de importacion de datos
    GtkWidget *dataimport_label;    // Etiqueta para importacion de datos
    GtkWidget *dataimport_picker;   // Seleccionador para importacion de datos
    GtkWidget *dataimport_button;   // Botón para importar datos
    GtkWidget *plotconfig_grid;     // Grid para configuracion de gráfica
    GtkWidget *plotconfig_frame;    // Frame para configuracion de gráfica
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
    gtk_grid_set_row_spacing (GTK_GRID (window_grid), 10);                 // Espaciamiento de filas
    gtk_grid_set_column_spacing (GTK_GRID (window_grid), 25);              // Espaciamiento de columnas
    gtk_grid_set_row_homogeneous(GTK_GRID(window_grid), FALSE);            // Filas homogeneas: Falso
    gtk_grid_set_column_homogeneous(GTK_GRID(window_grid), FALSE);          // Columnas homogeneas: Verdadero

    // Contener
    gtk_container_add (GTK_CONTAINER (window), window_grid);               // Contener el grid en window

    // (AUXILIAR, etiqueta al grid para visualizarlo)
//    for (int i = 0; i < 2; i++) {
//	for (int j = 0; j < 4; j++) {
//	    GtkWidget *label = gtk_label_new("O");
//	    gtk_widget_set_hexpand(label, FALSE);
//	    gtk_grid_attach(GTK_GRID(window_grid), label, j, i, 1, 1);
//	}
//    }
    
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
//    for (int i = 0; i < 1; i++) {
//	for (int j = 0; j < 3; j++) {
//	    GtkWidget *label = gtk_label_new("##");
//	    gtk_widget_set_hexpand(label, FALSE);
//	    gtk_grid_attach(GTK_GRID(dataimport_grid), label, j, i, 1, 1);
//	}
//    }

    
    // *************** SELECCIONADOR DE ARCHIVOS ***************
    
    // Crear etiqueta
    dataimport_label = gtk_label_new("Archivo:");                           // Nombre de la etiqueta
    gtk_widget_set_halign (dataimport_label, GTK_ALIGN_START);              // ALineacion horizontal
    gtk_widget_set_valign (dataimport_label, GTK_ALIGN_CENTER);             // Alineacion vertical
    gtk_widget_set_hexpand (dataimport_label, FALSE);                       // Expandir: Falso

    // Crear seleccionador
    dataimport_picker = gtk_file_chooser_button_new ("Seleccione un archivo", GTK_FILE_CHOOSER_ACTION_OPEN);
    gtk_file_chooser_set_local_only(GTK_FILE_CHOOSER(dataimport_picker), FALSE);
    gtk_widget_set_size_request(dataimport_picker, 250, -1);
    gtk_widget_set_hexpand (dataimport_picker, FALSE);                  

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
    plot_container = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);  //BUSCAR

    // Propiedades
    gtk_widget_set_size_request(plot_container, 1500, 600);                        // Tamaño

    // Crear frame
    plot_frame = gtk_frame_new("Gráfica");
    
    // Contener
    gtk_container_add(GTK_CONTAINER(plot_frame), plot_container);             // Contener plot_container en el frame
    gtk_grid_attach(GTK_GRID(window_grid), plot_frame, 0, 1, 4, 1);           // Contener el frame en el grid
    
    // *************** BOTON VISUALIZACION ***************
    
    // Crear boton
    boton_visualizar = gtk_button_new_with_label ("Visualizar");

    //Propiedades
    gtk_widget_set_size_request(boton_visualizar, 150, 50);

    // Datos a pasar el callback
    Datos.plot_container = plot_container; //Pasar Widget de contenedor para gráfica
    
    g_signal_connect (boton_visualizar, "clicked", G_CALLBACK (visualizar), &Datos);
    gtk_grid_attach(GTK_GRID(window_grid), boton_visualizar, 3, 0, 1, 1);

    
    // ********** CONTENEDOR PARA CONFIGURACION DE GRAFICA *************
    
    // Crear grid
    plotconfig_grid = gtk_grid_new ();

    // Propiedades del grid
    gtk_grid_set_row_spacing (GTK_GRID (plotconfig_grid), 5);              // Espaciamiento de filas
    gtk_grid_set_column_spacing (GTK_GRID (plotconfig_grid), 10);           // Espaciamiento de columnas
    gtk_grid_set_column_homogeneous(GTK_GRID(plotconfig_grid), FALSE);      // Filas homogeneas: Falso
    gtk_grid_set_column_homogeneous(GTK_GRID(plotconfig_grid), FALSE);      // Columnas homogeneas: Falso
    gtk_container_set_border_width (GTK_CONTAINER (plotconfig_grid), 5);   // Borde

    // Crear frame
    plotconfig_frame = gtk_frame_new("Configuración de gráfica");

    // Contener
    gtk_container_add(GTK_CONTAINER(plotconfig_frame), plotconfig_grid);    // Contener grid en el frame
    gtk_grid_attach(GTK_GRID(window_grid), plotconfig_frame, 1, 0, 2, 1);   //Contener frame en el grid principal
    // Widgets

    GtkWidget * label_entrada_titulo = gtk_label_new("Título:");                // Etiqueta titulo
    gtk_widget_set_halign (label_entrada_titulo, GTK_ALIGN_START);              // Alineacion horizontal
    gtk_widget_set_valign (label_entrada_titulo, GTK_ALIGN_CENTER);             // Alineacion vertical
    gtk_widget_set_hexpand (label_entrada_titulo, FALSE);                       // Expandir: Falso
    gtk_grid_attach(GTK_GRID(plotconfig_grid), label_entrada_titulo, 0, 0, 1, 1);

    GtkWidget * entrada_titulo = gtk_entry_new();                               // Widget para título de grafica
    gtk_entry_set_placeholder_text(GTK_ENTRY(entrada_titulo), "Título del gráfico");
    gtk_grid_attach(GTK_GRID(plotconfig_grid), entrada_titulo, 1, 0, 7, 1);
    gtk_widget_set_hexpand (entrada_titulo, TRUE);
    gtk_widget_set_size_request(entrada_titulo, 75, -1);

    GtkWidget * label_entrada_xlabel = gtk_label_new("Eje X:");        // Etiqueta eje X
    gtk_widget_set_halign (label_entrada_xlabel, GTK_ALIGN_START);              // Alineacion horizontal
    gtk_widget_set_valign (label_entrada_xlabel, GTK_ALIGN_CENTER);             // Alineacion vertical
    gtk_widget_set_hexpand (label_entrada_xlabel, FALSE);                       // Expandir: Falso
    gtk_grid_attach(GTK_GRID(plotconfig_grid), label_entrada_xlabel, 0, 1, 1, 1);

    GtkWidget * entrada_xlabel = gtk_entry_new();                               // Widget para etiqueta de eje X
    gtk_entry_set_placeholder_text(GTK_ENTRY(entrada_xlabel), "Etiqueta eje X"); 
    gtk_grid_attach(GTK_GRID(plotconfig_grid), entrada_xlabel, 1, 1, 3, 1);
    gtk_widget_set_hexpand(entrada_xlabel, FALSE);
    gtk_widget_set_size_request(entrada_xlabel, 75, -1);

    GtkWidget * label_entrada_ylabel = gtk_label_new("Eje Y:");        // Etiqueta eje Y
    gtk_widget_set_halign (label_entrada_ylabel, GTK_ALIGN_START);              // Alineacion horizontal
    gtk_widget_set_valign (label_entrada_ylabel, GTK_ALIGN_CENTER);             // Alineacion vertical
    gtk_widget_set_hexpand (label_entrada_ylabel, FALSE);                       // Expandir: Falso
    gtk_grid_attach(GTK_GRID(plotconfig_grid), label_entrada_ylabel, 4, 1, 1, 1);

    GtkWidget * entrada_ylabel = gtk_entry_new();                               // Widget para etiqueta de eje Y
    gtk_entry_set_placeholder_text(GTK_ENTRY(entrada_ylabel), "Etiqueta eje Y");
    gtk_grid_attach(GTK_GRID(plotconfig_grid), entrada_ylabel, 5, 1, 3, 1);
    gtk_widget_set_hexpand(entrada_ylabel, FALSE);
    gtk_widget_set_size_request(entrada_ylabel, 75, -1);

    GtkWidget * label_boton_color = gtk_label_new("Color de línea:");           // Etiqueta color de linea
    gtk_widget_set_halign (label_boton_color, GTK_ALIGN_START);              // Alineacion horizontal
    gtk_widget_set_valign (label_boton_color, GTK_ALIGN_CENTER);             // Alineacion vertical
    gtk_widget_set_hexpand (label_boton_color, FALSE);                       // Expandir: Falso
    gtk_grid_attach(GTK_GRID(plotconfig_grid), label_boton_color, 8, 1, 1, 1);
    
    GtkWidget * boton_color = gtk_color_button_new();                           // Widget para esoger color
    gtk_color_button_set_title(GTK_COLOR_BUTTON(boton_color), "Seleccionar color de línea"); // Titulo de boton
    gtk_grid_attach(GTK_GRID(plotconfig_grid), boton_color, 9, 1, 1, 1);

    GtkWidget * label_grosor = gtk_label_new("Grosor de línea:");        // Etiqueta grosor de linea
    gtk_widget_set_halign (label_grosor, GTK_ALIGN_START);              // Alineacion horizontal
    gtk_widget_set_valign (label_grosor, GTK_ALIGN_CENTER);             // Alineacion vertical
    gtk_widget_set_hexpand (label_grosor, FALSE);                       // Expandir: Falso
    gtk_grid_attach(GTK_GRID(plotconfig_grid), label_grosor, 8, 2, 1, 1);

    GtkWidget * grosor = gtk_spin_button_new_with_range(1, 10, 0.5); // Widget grosor de linea rango 1 a 10
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(grosor), 2);              // Grosor 2 por defecto
    gtk_grid_attach(GTK_GRID(plotconfig_grid), grosor, 9, 2, 1, 1);
    
    GtkWidget * label_xmin = gtk_label_new("Xmin:");                    // Etiqueta grosor de Xmin
    gtk_widget_set_halign (label_xmin, GTK_ALIGN_START);              // Alineacion horizontal
    gtk_widget_set_valign (label_xmin, GTK_ALIGN_CENTER);             // Alineacion vertical
    gtk_widget_set_hexpand (label_xmin, FALSE);                       // Expandir: Falso
    gtk_grid_attach(GTK_GRID(plotconfig_grid), label_xmin, 0, 2, 1, 1);

    GtkWidget * entrada_xmin = gtk_entry_new();                     // Widget para entrada Xmin
    gtk_entry_set_placeholder_text(GTK_ENTRY(entrada_xmin), "Xmin");
    gtk_entry_set_width_chars(GTK_ENTRY(entrada_xmin), 5);
    gtk_widget_set_hexpand(entrada_xmin, FALSE);
    gtk_grid_attach(GTK_GRID(plotconfig_grid), entrada_xmin, 1, 2, 1, 1);

    GtkWidget * label_xmax = gtk_label_new("Xmax:");                    // Etiqueta grosor de Xmax
    gtk_widget_set_halign (label_xmax, GTK_ALIGN_START);              // Alineacion horizontal
    gtk_widget_set_valign (label_xmax, GTK_ALIGN_CENTER);             // Alineacion vertical
    gtk_widget_set_hexpand (label_xmax, FALSE);                       // Expandir: Falso
    gtk_grid_attach(GTK_GRID(plotconfig_grid), label_xmax, 2, 2, 1, 1);

    GtkWidget * entrada_xmax = gtk_entry_new();                     // Widget para entrada Xmax
    gtk_entry_set_placeholder_text(GTK_ENTRY(entrada_xmax), "Xmax");
    gtk_entry_set_width_chars(GTK_ENTRY(entrada_xmax), 5);
    gtk_widget_set_hexpand(entrada_xmax, FALSE);
    gtk_grid_attach(GTK_GRID(plotconfig_grid), entrada_xmax, 3, 2, 1, 1);

    GtkWidget * label_ymin = gtk_label_new("Ymin:");                    // Etiqueta grosor de Ymin
    gtk_widget_set_halign (label_ymin, GTK_ALIGN_START);              // Alineacion horizontal
    gtk_widget_set_valign (label_ymin, GTK_ALIGN_CENTER);             // Alineacion vertical
    gtk_widget_set_hexpand (label_ymin, FALSE);                       // Expandir: Falso
    gtk_grid_attach(GTK_GRID(plotconfig_grid), label_ymin, 4, 2, 1, 1);

    GtkWidget * entrada_ymin = gtk_entry_new();                     // Widget para entrada Ymin
    gtk_entry_set_placeholder_text(GTK_ENTRY(entrada_ymin), "Ymin");
    gtk_entry_set_width_chars(GTK_ENTRY(entrada_ymin), 5);
    gtk_widget_set_hexpand(entrada_ymin, FALSE);
    gtk_grid_attach(GTK_GRID(plotconfig_grid), entrada_ymin, 5, 2, 1, 1);

    GtkWidget * label_ymax = gtk_label_new("Ymax:");                    // Etiqueta grosor de Ymax
    gtk_widget_set_halign (label_ymax, GTK_ALIGN_START);              // Alineacion horizontal
    gtk_widget_set_valign (label_ymax, GTK_ALIGN_CENTER);             // Alineacion vertical
    gtk_widget_set_hexpand (label_ymax, FALSE);                       // Expandir: Falso
    gtk_grid_attach(GTK_GRID(plotconfig_grid), label_ymax, 6, 2, 1, 1);

    GtkWidget * entrada_ymax = gtk_entry_new();                     // Widget para entrada Ymax
    gtk_entry_set_placeholder_text(GTK_ENTRY(entrada_ymax), "Ymax");
    gtk_entry_set_width_chars(GTK_ENTRY(entrada_ymax), 5);
    gtk_widget_set_hexpand(entrada_ymax, FALSE);
    gtk_grid_attach(GTK_GRID(plotconfig_grid), entrada_ymax, 7, 2, 1, 1);
    

    // (AUXILIAR, etiqueta al grid para visualizarlo)
//    for (int i = 0; i < 3; i++) {
//	for (int j = 0; j < 10; j++) {
//	    GtkWidget *label = gtk_label_new("#");
//	    gtk_widget_set_hexpand(label, FALSE);
//	    gtk_grid_attach(GTK_GRID(plotconfig_grid), label, j, i, 1, 1);
//	}
//    }

    
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
