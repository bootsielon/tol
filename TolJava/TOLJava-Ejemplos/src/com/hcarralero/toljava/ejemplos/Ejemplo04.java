package com.hcarralero.toljava.ejemplos;

//Se deben importar del paquete com.hcarralero.toljava todas las clases a usarse,
//para este ejemplo se requieren las clases: TOLMachine, TOLMachineException, TOLVMatrix y TOLVariable.
import com.hcarralero.toljava.TOLMachine;
import com.hcarralero.toljava.TOLMachineException;
import com.hcarralero.toljava.TOLVMatrix;
import com.hcarralero.toljava.TOLVariable;

/**
 * Tercer Ejemplo de TOLJava para Consola:
 *  1) Se crea una instancia de TOLMachine.
 *  2) Se ejecuta una sentencia TOL para definir 3 variables de tipo VMatrix: ma, mb y mc
 *  3) Se procesa el resultado para determinar si se produjo algún error.
 *  4) Se obtiene la variable "mc" definida en TOL y de ella se toma la matriz de sus valores.
 *  4) Se recorre la matriz obtenida y se muestra por pantalla.
 *
 * @author Humberto Andrés Carralero López (hcarralero@gmail.com)
 * @version 1.0
 * @see com.hcarralero.toljava.TOLMachine, com.hcarralero.toljava.TOLVariable
 */
public class Ejemplo04 {

    /**
     * @param args los argumenetos de la línea de comandos.
     */
    public static void main(String[] args) {
        //Se crea una instancia de la clase TOLMachine.
        TOLMachine tm= new TOLMachine();
        //En una cadena se almacena el código TOL que se ejecutará; en este caso
        //el código es para definir las variables tipo VMatrix: ma, mb y mc.
        String code= "VMatrix ma= Rand(4, 3, 0, 1); VMatrix mb= SetCol( [[2,3,4,7]] ) | SetCol( [[1,5,7,9]] ) | SetCol( [[3,2,0,1]] ); VMatrix mc=ma+mb;";
        Object res[]= tm.execute(code);
        //Se invoca el intérprete de TOL para ejecutar el código pasado por parámetro.
        //El resultado es un arreglo de cadenas.
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
                    //Se obtiene de TOL la variable mc y se muestra por pantalla.
                    TOLVariable varmc= tm.getVariable("mc");
                    //Se hace cast al valor de la variable para convertirlo al tipo correcto: VMatrix.
                    TOLVMatrix valmc= (TOLVMatrix)varmc.getValue();
                    //Se obtiene el número filas y el de columnas de la matriz.
                    int rows= valmc.getRows();
                    int cols= valmc.getColumns();
                    double values[][]= valmc.getValues();
                    for (int i= 0; i<rows; ++i) {
                        //Se cambia de línea antes de cada fila
                        System.out.println();
                        for (int j= 0; j<cols; ++j) {
                            //Se imprime cada celda de la matriz
                            System.out.print(values[i][j]+"\t");
                        }
                    }
                }
            } catch (NumberFormatException ex) {
                //Algo inusual (ERROR INTERNO DE TOLJava) ha ocurrido.
                System.out.println("ERROR DE ACCESO AL NÚMERO DE ERRORES");
            } catch (TOLMachineException ex) {
                //Se ha producido un error en el acceso a la Máquina Virtual de TOL.
                System.out.println("Error accediendo a TOL: "+ex.getMessage());
            }
        }
    }
    
}
