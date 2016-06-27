#Marco teórico
##Análisis preliminar

##Justificaciones
1. ¿Qué tipo de sockets decidió emplear?

Para realizar la transmisión de la información se decidió utilizar Protocol de Datagramas de Usuario (User Datagram Protocol en inglés), un protocolo no orientado a conexión. El Protocolo UDP es un protocolo de la capa de transporte para uso con el protocolo IP de la capa de red. El protocolo UDP proveé un servicio de intercambio de datagramas a través de la red en modo Best-Effort, es decir, que no puede asegurar la entrega de los datagramas o paquetes. 

Debido a que se requiere un tiempo de respuesta inmediato para minimizar, en lo posible, el tiempo de espera en la entra o salida de los automoviles en el estacionamiento, es necesario que la velocidad de transmisión del paquete se maximize. UDP se suele utilizar cuando se buscan transmisiones con una cantidad de información baja en los paquetes y altas velocidades de transferencia (aunque se puedan perder algunos paquetes como efecto adverso) por lo que para el caso actual, se puede satisfacer la solicitud. 

El protocolo TCP, sin embargo, también pudiese satisfacer los requisitos de este proyecto aunque, debido los mecanismos de establecer y verificar la conexión, no sería el tipo de socket ideal para el requerimiento.

2. Identifique todos los mensajes del sistema, indicando: el formato del mismo,
su tamaño en bytes, quién genera el mensaje y quién lo recibe y procesa.

El cliente...

Cuando el servidor recibe una petición de un ticket nuevo por parte de un cliente, el primero envía un mensaje de 16 bytes en formato BDDMMYYYYHHMMSSS
el cual se encuentra representado en el siguiente formato:

* B: Booleano indicando si hay puestos disponibles
* D: Día en el cual se creó el ticket
* M: Mes
* Y: Año
* H: Hora
* M: Minutos
* SSS: Código del ticket, el cual es un número entre el 000 y el 199 ó XXX en el caso de que no se haya generado el ticket debido a limite de carros alcanzado.


3. Realice el diseño completo del protocolo de comunicación que construya y
describa como opera el mismo, debe usar UDP como capa de transporte.
Incluya el punto de vista de los cambios de estado de las entidades que se
comunican. Se sugiere usar diagramas de máquinas de estados finitos y
facilitar la explicación de como es la actividad del protocolo.

4. Describa aspectos del proyecto que funcionan según el enunciado y cuales
no. Cualquier requerimiento no desarrollado o que contenga fallas, deberá
ser señalado claramente.

5. La implementación es en lenguaje C.

6. Opcional: Construya un aplicación confiable, que garantice la entrega, evite
información duplicada y pérdida de información. Todos estos controles
deben estar en capa de aplicación.