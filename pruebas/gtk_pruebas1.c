#include <gtk/gtk.h>

static void activate (GtkApplication* app, gpointer user_data) {
  GtkWidget *window;
  GtkWidget *grid;
  GtkWidget *plot_area;
  GtkWidget *plot_frame;
  
  window = gtk_application_window_new (app);
  gtk_window_set_title (GTK_WINDOW (window), "Visualizador de datos");
  //gtk_window_set_default_size (GTK_WINDOW (window), 1200, 800);
  //gtk_window_maximize(GTK_WINDOW (window));

  grid = gtk_grid_new ();

  // Configurar cantidad de filas y columnas del grid
  for (int i = 0; i<8; i++) {
      gtk_grid_insert_row (GTK_GRID(grid), i);
  }
  for (int i = 0; i<3; i++) {
      gtk_grid_insert_column (GTK_GRID(grid), i);
  }

  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 3; j++) {
        GtkWidget *label = gtk_label_new("Grid");
        gtk_grid_attach(GTK_GRID(grid), label, j, i, 1, 1);
    }
  }

  //Permitir tamaños distintos de filas y columnas del grid
  gtk_grid_set_row_homogeneous(GTK_GRID(grid), FALSE);
  gtk_grid_set_column_homogeneous(GTK_GRID(grid), FALSE);

  gtk_container_add (GTK_CONTAINER (window), grid);

  plot_area = gtk_drawing_area_new ();
  //gtk_widget_set_size_request(plot_area, 600, 400);
  //gtk_widget_set_name(plot_area, "plot_area");
  plot_frame = gtk_frame_new("Gráfica");

  gtk_container_add(GTK_CONTAINER(plot_frame), plot_area);

  //gtk_grid_attach(GTK_GRID(grid), plot_frame, 0, 4, 3, 4);


  gtk_widget_set_size_request(plot_frame, 1000, 500);
  gtk_grid_attach(GTK_GRID(grid), plot_frame, 0, 4, 3, 4);
  
  gtk_container_set_border_width (GTK_CONTAINER (window), 25);
  
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
