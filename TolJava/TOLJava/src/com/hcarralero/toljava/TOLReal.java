
package com.hcarralero.toljava;

/**
 * Representa a un objeto TOL de tipo Real.
 *
 * @author Humberto Andrés Carralero López (hcarralero@gmail.com)
 * @version 1.0
 */
public class TOLReal extends TOLLObject {

    private double value= 0.0;

    /**
     * Construye un objeto de tipo <code>TOLReal</code> a partir de un nḿero real
     *
     * @param value el valor real que representa
     */
    public TOLReal(double value) {
        this.value = value;
    }

    /**
     * Obtiene el valor numérico real almacenado
     *
     * @return el valor numérico real almacenado
     */
    public double getValue() {
        return value;
    }

    /**
     * Cambia el valor numérico real almacenado por el valor pasado como parámetro
     *
     * @param value nuevo valor numérico real
     */
    public void setValue(double value) {
        this.value = value;
    }

    /**
     * Determina si el objeto corresponde al tipo Real de TOL
     *
     * @return <code>true</code> ya que el objeto corresponde al tipo Real de TOL
     */
    @Override
    public boolean isReal() {
        return true;
    }

    /**
     * Compara a este objeto con otro
     
     * @param obj el objeto con el que se comparará
     * @return <code>true</code> si el objeto pasado como parámetro es de tipo <code>TOLReal</code> y contiene un valor numérico real idéntico
     */
    @Override
    public boolean equals(Object obj) {
        if (obj == null) {
            return false;
        }
        if (getClass() != obj.getClass()) {
            return false;
        }
        final TOLReal other = (TOLReal) obj;
        if (this.value == other.value) {
            return true;
        }
        return false;
    }

    /**
     * Calcula el código hash asignado al objeto
     *
     * @return el código hash asignado al objeto
     */
    @Override
    public int hashCode() {
        int hash = 5;
        hash = 67 * hash + (int) (Double.doubleToLongBits(this.value) ^ (Double.doubleToLongBits(this.value) >>> 32));
        return hash;
    }

    /**
     * Obtiene una representación como cadena de texto del valor numérico real
     *
     * @return la representación como cadena de texto del valor numérico real
     */
    @Override
    public String toString() {
        return ""+this.value;
    }

}
