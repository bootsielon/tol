
package com.hcarralero.toljava;

/**
 * <p>Representa una referencia a un objeto de tipo Serie definido en TOL.</p>
 *
 * @author Humberto Andrés Carralero López (hcarralero@gmail.com)
 * @version 1.0
 */
public class TOLRSerie extends TOLRObject {

    /**
     *  Construye un objeto de tipo <code>TOLRSerie</code>
     * 
     * @param tm el objeto <code>TOLMachine</code> al que está ligado
     * @param varname nombre de la variable en TOL
     */
    public TOLRSerie(TOLMachine tm, String varname) {
        super(tm, varname);
    }

    /**
     * Determina si el objeto corresponde al tipo Serie de TOL
     *
     * @return <code>true</code> ya que el objeto corresponde al tipo Serie de TOL
     */
    @Override
    public boolean isSerie() {
        return false;
    }

    /**
     * Obtiene el valor de la serie en una fecha dada
     *
     * @param date Fecha de la que desea el valor
     * @return el valor de la serie en una fecha dada
     * @throws com.hcarralero.toljava.TOLMachineException cuando hay algún error en el acceso a TOL
     */
    public double getValue(TOLDate date) throws TOLMachineException {
        return this.tm.getSerieValue(this.varname, date);
    }

    /**
     * Obtiene el valor de la serie en una posición dada
     *
     * @param index Posición de la que desea el valor
     * @return el valor de la serie en una fecha dada
     * @throws com.hcarralero.toljava.TOLMachineException cuando hay algún error en el acceso a TOL
     */
    public double getValue(int index) throws TOLMachineException {
        return this.tm.getSerieValue(this.varname, index);
    }

    /**
     * Obtiene un arreglo de elementos de la serie a partir de una fecha dada
     *
     * @param date Fecha inicial a partir de la que obtendrán los elementos
     * @param size Tamaño del arreglo a obtener
     * @return el arreglo de elementos de la serie a partir de una fecha dada
     * @throws com.hcarralero.toljava.TOLMachineException cuando hay algún error en el acceso a TOL
     */
    public TSItem[] getItems(TOLDate date, int size) throws TOLMachineException {
        return this.tm.getSerieItems(this.varname, date, size);
    }

    /**
     * Obtiene un arreglo de elementos de la serie a partir de una posición dada
     *
     * @param index Posición inicial a partir de la que obtendrán los elementos
     * @param size Tamaño del arreglo a obtener.
     * @return el arreglo de elementos de la serie a partir de una posición dada
     * @throws com.hcarralero.toljava.TOLMachineException cuando hay algún error en el acceso a TOL
     */
    public TSItem[] getItems(int index, int size) throws TOLMachineException {
        return this.tm.getSerieItems(this.varname, index, size);
    }

    /**
     * Obtiene la primera fecha de la serie
     *
     * @return la primera fecha de la serie
     * @throws com.hcarralero.toljava.TOLMachineException cuando hay algún error en el acceso a TOL
     */
    public TOLDate getFirstDate() throws TOLMachineException {
       return this.tm.getSerieFirstDate(this.varname);
    }

    /**
     * Obtiene la última fecha de la serie
     *
     * @return la primera fecha de la serie
     * @throws com.hcarralero.toljava.TOLMachineException cuando hay algún error en el acceso a TOL
     */
    public TOLDate getLastDate() throws TOLMachineException {
       return this.tm.getSerieLastDate(varname);
    }

    /**
     * Compara a este objeto con otro
     *
     * @param obj el objeto con el que se comparará
     * @return <code>true</code> si el objeto pasado como parámetro es de tipo <code>TOLRSerie</code> y contiene una serie idéntica
     */
    @Override
    public boolean equals(Object obj) {
        if (obj == null) {
            return false;
        }
        if (getClass() != obj.getClass()) {
            return false;
        }
        final TOLRSerie other = (TOLRSerie) obj;
        return true;
    }

    /**
     * Calcula el código hash asignado al objeto
     *
     * @return el código hash asignado al objeto
     */
    @Override
    public int hashCode() {
        int hash = 7;
        return hash;
    }

    /**
     * Obtiene una representación como cadena de texto de la Serie en la que incluye el nombre
     *
     * @return la representación como cadena de texto de la Serie en la que incluye el nombre
     */
    @Override
    public String toString() {
        return "Serie@"+varname;
    }

}
