package com.hcarralero.toljava.ejemplos;

//Se deben importar del paquete com.hcarralero.toljava todas las clases a usarse,
//para este ejemplo se requieren las clases: TOLMachine, TOLMachineException, TOLReal y TOLCode.
import com.hcarralero.toljava.TOLMachine;
import com.hcarralero.toljava.TOLMachineException;
import com.hcarralero.toljava.TOLDate;
import com.hcarralero.toljava.TOLDateException;
import com.hcarralero.toljava.TSItem;
import java.util.Calendar;
import java.util.Date;

/**
 * 7mo Ejemplo de TOLJava para Consola:
 *  1) Se crea una instancia de TOLMachine.
 *  2) Se ejecuta una sentencia TOL para definir la serie ser1.
 *  3) Se procesa el resultado.
 *  4) Se muestran los mensajes devueltos por el intéprete de TOL.
 *  5) Se obtiene un arreglo de valores (tsis1) de la serie a partir una fecha específica.
 *  6) Se muestran los valores obtenidos.
 *
 * @author Humberto Andrés Carralero López (hcarralero@gmail.com)
 * @version 1.0
 * @see com.hcarralero.toljava.TOLMachine, com.hcarralero.toljava.TOLMachineException,
 *      com.hcarralero.toljava.TOLReal, com.hcarralero.toljava.TOLCode.
 */
public class Ejemplo07 {

    /**
     * @param args los argumenetos de la línea de comandos.
     */
    public static void main(String[] args) {
        //Se crea una instancia de la clase TOLMachine.
        TOLMachine tm= new TOLMachine();
        //En una cadena se almacena el código TOL que se ejecutará; en este caso
        //el código es para definir una serie aleatoria con valores para todos los días del año 2009.
        String code= "Serie ser1 = SubSer( Gaussian( 0, 1 ), y2009m1d1, y2009m12d31 );";
        //Se invoca el intérprete de TOL para ejecutar el código pasado por parámetro.
        //El resultado es un arreglo de cadenas Object.
        Object res[]= tm.execute(code);
        System.out.println();
        if (res==null) {
            //Si el resultado es nulo, entonces se ha producido un error en el acceso a TOL.
            System.out.println("ERROR ACCEDIENDO A TOL->execute");
        } else {
            //Se ha logrado acceder al intérprete y ejecutar el código.
            int ne= 0;
            try {
                //La primera cadena representa el número de errores detectado por el Intérprete de TOL.
                ne= Integer.parseInt((String)res[0]);
                if (ne>0) {
                    //El intérprete de TOL reportó al menos un error.
                    System.out.println("Se ha(n) detectado "+ne+" error(es)");
                } else {
                    //El intérprete de TOL no reportó errores.
                    System.out.println("No se han detectado errores");
                }
                //Los siguientes objetos que conforman la respuesta del intérprete pueden ser:
                //Warnings, Errores o mensajes generados por el código TOL ejecutado.
                //Para procesar los mensajes generados debe comprender el formato de
                //estos mensajes. (Consulte la documentación de TOL).
                for (int i= 1; i<res.length; ++i) {
                    //Cada objeto es una cadena
                    String strRes= (String)res[i];
                    //Se obtiene la longitud de la cadena.
                    int l= strRes.length();
                    if (strRes.charAt(0)=='<'&&strRes.charAt(2)=='>'&&(strRes.charAt(1)=='E'||strRes.charAt(1)=='W')&&l>=7) {
                        //En este caso el mensaje es un 'W'arning o un 'E'rror.
                        System.out.println("\t"+strRes.substring(3, l-5));
                    } else {
                        //En este caso es un mensaje común.
                        System.out.println("\t"+strRes);
                    }
                }
                //Se crea una variable fecha con valor dentro del rango de fechas de la serie
                TOLDate date= new TOLDate(2009, 3, 11);
                //Se obtiene un arreglo de a lo sumo 10 valores de la serie a partir de la fecha dada
                TSItem tsis1[]= tm.getSerieItems("ser1", date, 10);
                //Se imprime el número real de elementos obtenidos (pudiera ser inferior a 10)
                System.out.println("Número de elementos leídos: "+tsis1.length);
                //Se crea una variable calendario usado para posteriormente imprimir las fechas de la serie
                Calendar cal= Calendar.getInstance();
                //Se recorre el arreglo obtenido
                for (int i= 0; i<tsis1.length; ++i) {
                    //Se obtiene la fecha de cada item
                    TOLDate tsid= tsis1[i].getDate();
                    //Se establece en el calendario la fecha obtenida para su impresión
                    cal.set(tsid.getYear(), tsid.getMonth(), tsid.getDay(), tsid.getHour(), tsid.getMinute(), (int)tsid.getSecond());
                    //Se imprime la fecha y el valor de cada item de la serie
                    System.out.println("["+new Date(cal.getTimeInMillis()).toString()+"]==>["+tsis1[i].getValue()+"]");
                }
            } catch (NumberFormatException ex) {
                //Algo inusual (ERROR INTERNO DE TOLJava) ha ocurrido.
                System.out.println("ERROR DE ACCESO AL NÚMERO DE ERRORES");
            } catch (TOLMachineException ex) {
                //Algún error ha ocurrido en el acceso a la variable f.
                System.out.println("ERROR accediendo a la serie ser1: "+ex.getMessage());
            } catch (TOLDateException ex) {
                //Algún error ha ocurrido en el manejo de la variable de fecha (date).
                System.out.println("ERROR accediendo a la serie ser1: "+ex.getMessage());
            }
        }
    }
}
