#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

/* =====================================================
   PREGUNTA 1: Procesamiento concurrente de pedidos
   ===================================================== */

void pregunta1() {

    double monto = 250.0;
    int cantidad = 10;

    printf("[INICIO] PID=%d\n", getpid());
    fflush(stdout);
    // TODO 1: Crear un proceso hijo utilizando fork().
pid_t pid = fork();
    // TODO 2: Gestionar el error de creación.
if (pid==-1){
perror("fork");
return;
}
    // TODO 3: Implementar las operaciones del proceso hijo
    //         (monto += 50, esperar 1 segundo, imprimir PID y monto).
if (pid==0){
monto += 50;
sleep(1);
printf("[HIJO] PID=%d\n", getpid());
printf("[HIJO] monto=%f\n", monto);
}
    // TODO 4: Implementar las operaciones del proceso padre
    //         (cantidad += 5, imprimir PID y cantidad).
else {
cantidad += 5;
printf("[PADRE] PID=%d\n", getpid());
printf("[PADRE] cantidad=%d\n", cantidad);
}
    // TODO 5: Explicar en un comentario por qué el hijo
    //         no modifica las variables del padre.
//Porque con el fork se crean dos procesos diferentes cada uno con sus respectivas variables,
//el proceso hijo solo modifica las variables de su proceso, no altera ni modifica las variables del proceso padre.
}

/* =====================================================
   PREGUNTA 2: Supervisión de una tarea de respaldo
   ===================================================== */
void pregunta2() {

    pid_t pid = fork();
    int status;
    if (pid==-1) {
        perror("fork");
        return;
    }
    if (pid==0) {
        printf("[HIJO] Iniciando respaldo...\n");
        sleep(2);
        exit(0);
    } else {
        printf("[PADRE] Esperando finalización del respaldo...\n");
        // TODO: Esperar al hijo mediante waitpid().
	int terminado = 0;
	pid_t res = waitpid(pid, &status, WNOHANG);
        // TODO: Comprobar si waitpid() falló.
	if (res==-1){
	perror("waitpid");
	return;
	}
	while(!terminado){
        // TODO: Comprobar si el hijo terminó normalmente.
	if (res>0){
	//TODO: Obtener e imprimir el código de salida.
	printf("[HIJO] El respaldo termino correctamente con codigo de salida %d\n", WEXITSTATUS(status));
	terminado = 1;
	}
	}
	if (WEXITSTATUS(status)==0){
	printf("[PADRE] Respaldo completado correctamente. \n");
	}
        // TODO: Mostrar si el respaldo fue exitoso o falló.
	else {
	printf("[PADRE] Error durante el respaldo. \n");
	}
    }
}

/* =====================================================
   PREGUNTA 3: Diagnóstico del sistema mediante execvp()
   ===================================================== */

void pregunta3() {

    pid_t pid;
    int status;
    char *argumentos[] = {
        // TODO: Completar los argumentos para uname -a.
        //       (recuerde qué va en la primera posición, argv[0])
	"uname",
	"-a",
        NULL
    };
    pid = fork();
    if (pid==-1) {
        perror("fork");
        return;
    }
    if (pid==0) {
        printf("[HIJO] Consultando información del sistema...\n");
        fflush(stdout);
        // TODO: Ejecutar uname mediante execvp().
	execvp(argumentos[0], argumentos);
        // TODO: Si execvp() falla, mostrar el error
        //       y terminar con código 127.
	perror("execvp");
	_exit(127);
    } else {
        // TODO: Esperar al hijo mediante waitpid().
	printf("[PADRE] Esperando al hijo...\n");
	waitpid(pid, &status, 0);
        if (/* TODO: comprobar terminación norma*/ WIFEXITED(status)==1) {
            // TODO: Mostrar código de salida.
	    printf("[PADRE] El hijo termino con codigo %d\n", WEXITSTATUS(status));
        } else if (/* TODO: comprobar terminación por señal */ WIFSIGNALED(status)) {
            // TODO: Mostrar número de señal.
	    printf("[PADRE] El hijo no termino por una señal %d\n", WTERMSIG(status));
        }
    }
}

/* =====================================================
   PREGUNTA 4: Monitoreo de múltiples procesos con WNOHANG
   ===================================================== */
void pregunta4() {
    pid_t pid1, pid2;
    int status1, status2;
    int terminado1 = 0;
    int terminado2 = 0;

    // Crear el primer proceso hijo.
    pid1 = fork();
    if (pid1==-1) {
        perror("fork");
        return;
    }
    if (pid1==0) {
        printf("[HIJO 1] Ejecutando tarea 1...\n");
        sleep(2);
        // TODO: Terminar con código 10.
        // No debe continuar ejecutando el código del padre.
	_exit(10);
	terminado1 = 1;
    }

    // Crear el segundo proceso hijo.
    pid2 = fork();
    if (pid2==-1) {
        perror("fork");
        // Esperar al primer hijo antes de abandonar la función.
        waitpid(pid1, &status1, 0);
        return;
    }
    if (pid2==0) {
        printf("[HIJO 2] Ejecutando tarea 2...\n");
        sleep(4);
        // TODO: Terminar con código 20.
        // No debe continuar ejecutando el código del padre.
	_exit(20);
	terminado2 = 1;
    }
    // A partir de aquí solamente continúa el proceso padre.
    while (!terminado1 || !terminado2) {
        // TODO: Consultar el estado del primer hijo
        //       utilizando waitpid() y WNOHANG
	pid_t res1;
	res1 = waitpid(pid1, &status1, WNOHANG);
        //       (solo si todavía no ha terminado).
	if (res1==0){
	printf("[PADRE] El hijo 1 aun no termina...\n");
	}
        // TODO: Si terminó, actualizar terminado1
        //       y mostrar su PID y código de salida.
	else if (res1>0){
	printf("[PADRE] El hijo 1 terminó, su pid es %d\n", getpid());
	printf("[PADRE] El hijo 1 terminó, su codigo de salida es %d\n", WEXITSTATUS(status1));
	terminado1 = 1;
	}
        // TODO: Consultar el estado del segundo hijo
        //       utilizando waitpid() y WNOHANG
	pid_t res2;
	res2 = waitpid(pid2, &status2, WNOHANG);
        //       (solo si todavía no ha terminado).
	if (res2==0){
	printf("[PADRE] El hijo 2 aun no termina...\n");
        // TODO: Si terminó, actualizar terminado2
        //       y mostrar su PID y código de salida.
	} else if (res2>0){
	printf("[PADRE] El hijo 2 terminó, su pid es %d\n", getpid());
	printf("[PADRE] El hijo 2 terminó, su codigo de salidad es %d\n", WEXITSTATUS(status2));
	}
        if (!terminado1 || !terminado2){
	printf("[PADRE] Supervisando tareas...\n");
	sleep(1);
	}
    }
    printf("[PADRE] Todas las tareas finalizaron.\n");

    // TODO: Explicar en un comentario qué ocurre si el padre consulta
    //       con waitpid() a un hijo que ya fue recogido.
//El proceso hijo por el que consulta ya no existe ya que al recogerla significa que ese ciclo se vuelve a integrar al proceso padre.
}
/* =====================================================
   PROGRAMA PRINCIPAL
   ===================================================== */

int main() {

    int opcion;
    printf("====================================\n");
    printf("     LABORATORIO CALIFICADO 04\n");
    printf("====================================\n");

    printf("1. Pregunta 1\n");
    printf("2. Pregunta 2\n");
    printf("3. Pregunta 3\n");
    printf("4. Pregunta 4\n");

    printf("Seleccione una pregunta: ");

    if (scanf("%d", &opcion) != 1) {
        printf("Entrada no valida.\n");
        return 1;
    }

    switch (opcion) {
        case 1:
            pregunta1();
            break;
        case 2:
            pregunta2();
            break;
        case 3:
            pregunta3();
            break;
        case 4:
            pregunta4();
            break;
        default:
            printf("Opcion no válida.\n");
    }
    return 0;
}
