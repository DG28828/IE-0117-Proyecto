# Visualizador de datos
Este programa es un visualizador de datos escrito en lenguaje de programación C, empleando la librería GTK3.

El programa permite importar datos de un archivo de texto, visulizar los datos y modificar el título, etiquetas de ejes de la gráfica, límites de ejes X e Y, así como el color y espesor de línea.

**Limitaciones:**
- El delimitador del archivo debe ser tab (\t), espacio ( ), coma (,) o punto y coma (;).
- Los datos a graficar deben estar en la primera y segunda columna del archivo.
- El archivo no debe contener encabezados.
- Solo se puede graficar una serie de datos a la vez.

**Requisitos del sistema:**
- Sistema operatico Linux (probado en distribuciones basadas en Debian/Ubuntu)
- Entorno de escritorio compatible con GTK3

**Dependencias:**
- GTK+ 3.0 (Probado en librería versión 3.24)
- Makefile (Probado en GNU Make 4.3)

**¿Como compilar?**
El archivo **Makefile** contiene las instrucciones de compilación, ejecución y verificación de errores. Las instrucciones brindadas son:

- **$make**: compila en un ejecutable resultante **visualizador**.
- **$make run**: compila y ejecuta el archivo **visualizador**.
- **$make clean**: borra los archivos .o resultantes de la compilación y el archivo compilado **visualizador**.
- **$make valgrind**: realiza un análisis en búsqueda de mal manejo de memoria.
- **$make deps**: instala las dependencias del programa.

En caso de correr únicamente **$make**, es posible ejecutar el archivo resultante con **$./visualizador**.