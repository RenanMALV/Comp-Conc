/* Aluno: Renan Mendanha */
/* Disciplina: Computacao Concorrente */
/* Prof.: Silvana Rossetto */
/* Laboratório: 7 */
/* -------------------------------------------------------------------*/

// rand
import java.util.concurrent.ThreadLocalRandom;

//classe da estrutura de dados (recurso) compartilhado entre as threads
class NumPares{
   //recurso compartilhado
   private int[] arr;
   private int num = 0; // contador de numeros pares
   public int NElem; // tamanho do array
   
   //construtor
   public NumPares(int[] arr) { 
      this.arr = arr; 
      this.NElem = arr.length;
   }

   //adiciona ao contador num se o elemento no índice index for par
   public synchronized void check(int index) { 
      if(arr[index]%2==0)
        num++; 
   }

   //retorna o numero de elementos pares
   public synchronized int get() { 
      return this.num; 
   }

}

//classe que estende Thread e implementa a tarefa de cada thread do programa 
class ThreadCheckPar extends Thread {
   //identificador da thread
   private int id;
   //objeto compartilhado com outras threads
   private NumPares numPs;
   //Número de threads
   private int N;
  
   //construtor
   public ThreadCheckPar(int tid, int N, NumPares numPs) { 
      this.id = tid; 
      this.numPs = numPs;
      this.N = N;
   }

   //metodo main da thread
   public void run() {
      System.out.println("Thread " + this.id + " iniciou!");
      for (int i=id; i<numPs.NElem; i+=N) {
         this.numPs.check(i);  
      }
      System.out.println("Thread " + this.id + " terminou!"); 
   }
}

//classe da aplicacao
class program {
    // num threads
    static final int N = 2;
    // num elementos
    static final int NElem = 1000000;

    public static void main (String[] args) {
        //reserva espaço para um vetor de threads
        Thread[] threads = new Thread[N];

        // cria um arry com números aleatórios
        int[] arr = new int[NElem];
        for(int i = 0; i<NElem; i++){
            arr[i] = (int) ThreadLocalRandom.current().nextInt(0, 1000000);
        }

        //cria uma instancia do recurso compartilhado entre as threads
        NumPares NPs = new NumPares(arr);

        //cria as threads da aplicacao
        for (int i=0; i<threads.length; i++) {
            threads[i] = new ThreadCheckPar(i, N, NPs);
        }

        //inicia as threads
        for (int i=0; i<threads.length; i++) {
            threads[i].start();
        }

        //espera pelo termino de todas as threads
        for (int i=0; i<threads.length; i++) {
            try { threads[i].join(); } catch (InterruptedException e) { return; }
        }

        System.out.println("Total de numeros pares no array = " + NPs.get()); 

        //verifica corretude
        if(verify(arr, NPs))
            System.out.println("Passou no teste de corretude");
        else
            System.out.println("Falhou no teste de corretude");
    }

    public static boolean verify(int[] arr, NumPares NPs){
        int cont = 0;
        for(int i = 0; i<NElem; i++){
            if(arr[i]%2==0)
                cont++;
        }

        return (cont == NPs.get());
    }
}
