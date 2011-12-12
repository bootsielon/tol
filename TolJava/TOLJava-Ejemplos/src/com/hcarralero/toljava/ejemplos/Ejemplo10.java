package com.hcarralero.toljava.ejemplos;

//Se deben importar del paquete com.hcarralero.toljava todas las clases a usarse,
//para este ejemplo se requieren las clases: TOLMachine, TOLMachineException, TOLDate y TOLDateException.
import com.hcarralero.toljava.TOLMachine;
import com.hcarralero.toljava.TOLMachineException;
import com.hcarralero.toljava.TOLObject;
import com.hcarralero.toljava.TOLRSet;
import com.hcarralero.toljava.TOLVariable;

/**
 * 10mo Ejemplo de TOLJava para Consola:
 *  1) Se crea una instancia de TOLMachine.
 *  2) Se ejecuta una sentencia TOL para definir el Set set1.
 *  3) Se procesa el resultado.
 *  4) Se muestran los mensajes devueltos por el intéprete de TOL.
 *  5) Se obtiene el elemento de la posición 3 y se muestra.
 *  6) Se obtiene el tamaño de set1 y se muestra.
 *  7) Se obtienen todos los elementos del conjunto y se muestran.
 *
 * @author Humberto Andrés Carralero López (hcarralero@gmail.com)
 * @version 1.0
 * @see com.hcarralero.toljava.TOLMachine, com.hcarralero.toljava.TOLMachineException,
 *      com.hcarralero.toljava.TOLReal, com.hcarralero.toljava.TOLCode.
 */
public class Ejemplo10 {

    /**
     * @param args los argumenetos de la línea de comandos.
     */
    public static void main(String[] args) {
        //Se crea una instancia de la clase TOLMachine.
        TOLMachine tm= new TOLMachine();
        //En una cadena se almacena el código TOL que se ejecutará; en este caso
        //el código es para definir un Set de 5 números reales"
//        String code= "Set  set1 = SetOfText(\"alfa01\", \"beta01\", \"alfa02\", \"beta03\", \"alfa04\");";
//        String code= "Set  set1 = SetOfReal(5, 1.4, .23, 456, 83.85);";
        String code= "#Require MMS;"+
                     "NameBlock forecast = MMS::Container::LoadFile(\"/home/hcarralero/Objetos-MMS/MPAPME.AutPEmMEm.Trim_20110606_BSR_1.0_Point__1.0.oza\");"+
                     "Set simInputs = Select(forecast::GetModel(?)::GetDataSet(?)::GetVariables(?), Real (NameBlock v) {"+
                     "TextSub(v::GetName(?), -5, -1) == \"_Comb\""+
                     "});"+
                     "Set inputs = EvalSet(simInputs, Set (NameBlock v) {"+
                     "Serie scenarioData = v::GetData(?);"+
                     "Text inputName = TextSub(v::GetName(?), 1, TextFind(v::GetName(?), \"_\")-1);"+
                     "NameBlock v0 = forecast::GetModel(?)::GetDataSet(?)::GetVariable(inputName);"+
                     "Serie inputData = v0::GetData(?);"+
                     "[[ inputName, inputData, scenarioData ]]"+
                     "});"+
                     "Set outputs= EvalSet(forecast::GetModel.Forecast(?)::GetSubmodels(?), Set (NameBlock submodel) {"+
                     "Text outputName = submodel::GetName(?);"+
                     "Serie outputData = submodel::GetObservations(?);"+
                     "Serie scenarioData = submodel::GetObservations.Forecast.Mean(?);"+
                     "Set outputExprterms= submodel::GetExpTerms(?);"+
                     "Set outputInputs = Select(outputExprterms, Real (NameBlock expr) {"+
                     "TextSub(expr::GetName(?), -9, -1) == \"_Comb_Ext\""+
                     "});"+
                     "Set simOutputInputsNames= EvalSet(outputInputs, Text (NameBlock oinput) {"+
                     "Text inputName = TextSub(oinput::GetName(?), 1, TextFind(oinput::GetName(?), \"_\")-1)"+
                     "});"+
                     "[[ outputName, outputData, scenarioData, simOutputInputsNames ]]"+
                     "});";
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
                TOLVariable toInputs= tm.getVariable("inputs");
                TOLObject to= toInputs.getValue();
                System.out.println("\tinputs==>"+to.toString());
                to= tm.getSetElement("inputs", 1);
                System.out.println("\tinputs[1])==>"+to.toString());
                int size= tm.getSetSize("inputs");
                //Se imprime el tamaño de "inputs"
                System.out.println("Cantidad de inputs=> ["+size+"]");
                //Se obtienen todos los elementos del conjunto
                TOLObject tos[]= tm.getSetElements("inputs");
                //Se imprime el arreglo que contiene los elementos del conjunto
                System.out.println("Elementos de inputs:");
                for (int i= 0; i<tos.length; ++i) {
                    System.out.println("\tinputs["+(i+1)+"]==>"+tos[i].toString());
                }
                for (int i= 0; i<size; ++i) {
                    System.out.println("\tinputs["+(i+1)+"]==>"+tm.getSetElement("inputs", i+1).toString());
                }
            } catch (NumberFormatException ex) {
                //Algo inusual (ERROR INTERNO DE TOLJava) ha ocurrido.
                System.out.println("ERROR DE ACCESO AL NÚMERO DE ERRORES");
            } catch (TOLMachineException ex) {
                //Algún error ha ocurrido en el acceso a la variable f.
                System.out.println("ERROR accediendo a 'inputs': "+ex.getMessage());
            }
        }
    }
}
