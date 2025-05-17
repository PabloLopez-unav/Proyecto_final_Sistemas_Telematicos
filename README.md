# Proyecto Final - Sistemas Telemáticos

Este proyecto fue desarrollado como parte del trabajo final de la asignatura **Sistemas Telemáticos** en la Universidad de Navarra. El objetivo es simular la arquitectura de un sistema distribuido para el control de un vehículo mediante módulos que se comunican entre sí a través de sockets TCP/IP.

## Estructura del Proyecto

pablolopez-unav-proyecto_final_sistemas_telematicos/
├── Accionamientos/
├── Cliente/
├── Motor/
└── Servidor_Luces/


Cada uno de los cuatro módulos es una aplicación independiente basada en **MFC (Microsoft Foundation Classes)** y tiene su propia solución de Visual Studio (`.sln`).

### Módulos

#### 1. Cliente

- Interfaz principal de usuario.
- Permite seleccionar y controlar distintos sistemas del vehículo.
- Envía comandos a los servidores (luces, motor, accionamientos).

#### 2. Accionamientos

- Simula el estado del freno, intermitentes izquierdo/derecho y el encendido general.
- Escucha en el puerto 5021.
- Usa una interfaz MFC para mostrar el estado y gestionar el inicio/parada del sistema.

#### 3. Motor

- Representa el sistema del motor del vehículo.
- Recibe el estado del encendido y de los accionamientos.
- Ejecuta la lógica correspondiente para encender/apagar y ajustar potencia o velocidad.

#### 4. Servidor_Luces

- Controla el estado de las luces.
- Se comunica con el cliente para activar/desactivar luces específicas.
- Muestra el estado en una interfaz gráfica.

## Tecnologías Utilizadas

- Lenguaje: **C++**
- Framework: **MFC (Microsoft Foundation Classes)**
- Comunicación: **Sockets TCP/IP (Winsock)**
- Entorno: **Visual Studio 2022**
- Sistema Operativo: **Windows**

## Cómo Ejecutar el Proyecto

1. Abre cada solución (`*.sln`) en Visual Studio.
2. Compila todos los proyectos (preferiblemente en modo `Debug`).
3. Ejecuta los servidores en el siguiente orden:
   - Servidor_Luces
   - Motor
   - Accionamientos
4. Ejecuta el Cliente.
5. Desde el Cliente, puedes interactuar con los diferentes módulos y observar la respuesta en tiempo real.

## Notas Adicionales

- Asegúrate de tener habilitado el soporte para sockets en MFC (`AfxSocketInit()`).
- Desactiva en los settings antes de crear el proyecto, en Advance, About Box. El proyecto es de tipo Dialogo. Luego también Hay que quitar una configuración en el porpio proyecto y poner **Not Set**, algo así.
- Todos los módulos están diseñados para ejecutarse en la misma máquina, pero podrían adaptarse para ejecutarse en red local modificando las IPs.
- La comunicación entre módulos se realiza mediante Modbus.

## Autor

Desarrollado por **Pablo y Joseba**, estudiante de Ingeniería en la Universidad de Navarra.

## Licencia

Este proyecto está destinado únicamente para fines académicos y educativos.
