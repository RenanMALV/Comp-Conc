/* lab-8 */
/* Renan Mendanha */
/* Disciplina: Computacao Concorrente */

/*
-Instruções para execução no PowerShell-
javac Lab8.java
java Lab8 | out-file -encoding utf8 "saida.py"
// Executando o programa, force uma interrupção para terminar
// para verificar a corretude, execute:
python3 .\saida.py
// os logs serão mostrados normalmente
// se não ocorrer avisos de erro, a corretude está garantida!
*/

// importa a implementação do Monitor que aplica logica do padrao leitores/escritores
import LeitorEscritor.LE;
import java.util.concurrent.ThreadLocalRandom;

// Classe do recurso compartilhado entre as threads.
class Numero {
	private int numero; // numero acessado entre as threads
	
	// Construtor
	Numero() {
		this.numero = 0; 
	}
	
	public int get() {
		return this.numero;
	}

	public void set(int n) {
		this.numero = n;
	}

	// Imprime se o número é par ou ímpar
	public void verificaParidade() {
    if (this.numero % 2 == 0)
      System.out.println("print(\"" + this.numero + " e um numero par\")");
    else
      System.out.println("print(\"" + this.numero + " e um numero impar\")");
	}
}

// Thread do tipo T1 - Escritora
class T1 extends Thread {
  int id; //identificador da thread
  LE monitor; //monitor do tipo LeitorEscritor
  Numero numero; //recurso compartilhado

  // Construtor
  T1 (int id, LE m, Numero numero) {
    this.id = id;
    this.monitor = m;
    this.numero = numero;
  }

  public void run () {
    try {
      while(true){
        this.monitor.EntraEscritor(this.id); 
        numero.set(numero.get()+1); // soma 1 ao número
        this.monitor.SaiEscritor(this.id);

        // simula um processament(para melhor visualização do log)
        // que dura entre meio segundo e 1 segundo
        sleep(ThreadLocalRandom.current().nextInt(500, 1001));
      }
    } catch (Exception e) { return; }
  }
}

// Thread do tipo T2 - Leitora
class T2 extends Thread {
  int id; //identificador da thread
  LE monitor; //monitor do tipo LeitorEscritor
  Numero numero; //recurso compartilhado

  // Construtor
  T2 (int id, LE m, Numero numero) {
    this.id = id;
    this.monitor = m;
    this.numero = numero;
  }

  public void run () {
    try {
      while(true) {
        this.monitor.EntraLeitor(this.id);
        numero.verificaParidade(); //printa se o numero é par ou ímpar
        this.monitor.SaiLeitor(this.id);

        // simula um processament(para melhor visualização do log)
        // que dura entre meio segundo e 1 segundo
        sleep(ThreadLocalRandom.current().nextInt(500, 1001)); 
      }
    } catch (Exception e) { return; }
  }
}

// Thread di tipo T3 - Leitora e Escritora
class T3 extends Thread {
  int id; //identificador da thread
  LE monitor; //monitor do tipo LeitorEscritor
  Numero numero; //recurso compartilhado

  // Construtor
  T3 (int id, LE m, Numero numero) {
    this.id = id;
    this.monitor = m;
    this.numero = numero;
  }

  public void run () {
    try {
      while(true){
        // Leitura
        this.monitor.EntraLeitor(this.id);
        System.out.println("print(\"" + "Numero = " + numero.get() + "\")"); //printa o valor do numero
        this.monitor.SaiLeitor(this.id);

        // simula um processamento que dura entre meio segundo e 2 segundos
        sleep(ThreadLocalRandom.current().nextInt(500, 2001)); 
        
        // Escrita
        this.monitor.EntraEscritor(this.id);
        numero.set(this.id); //numero = id
        this.monitor.SaiEscritor(this.id);
      }
    } catch (Exception e) { return; }
  }
}

// main program
public class Lab8 {
  // define numero de threads
  public static final int N_E = 2;  // Número de escritores
  public static final int N_L = 2;  // Número de leitores
  public static final int N_LE = 3; // Quantidade de threads leitoras/escritoras

  public static void main (String[] args) {
    
    //inicia o log de saida
    System.out.println("import verificaLE");
    System.out.println("le = verificaLE.LE()");

    LE monitor = new LE();  // instanciando o monitor
    Thread[] threads = new Thread[N_E+N_L+N_LE];  // Estrutura que guarda as threads
    Numero numero = new Numero(); // instanciando o recurso compartilhado

    int i; // iterador
    // instanciando uma thread do tipo T1 - escritora
    for (i=0; i<N_E; i++) {
      threads[i] = new T1(i+1, monitor, numero);
      threads[i].start(); 
    }

    // instanciando uma thread do tipo T3 - escritora/leitora
    for (; i<N_E+N_LE; i++) {
      threads[i] = new T3(i+1, monitor, numero);
      threads[i].start(); 
    }

    // instanciando uma thread do tipo T2 - leitora
    for (; i<N_E+N_LE+N_L; i++) {
        threads[i] = new T2(i+1, monitor, numero);
        threads[i].start(); 
    }
  }
}