package com.hcarralero.toljava;

/**
 * Representa a un objeto TOL de tipo Code.
 *
 * @author Humberto Andrés Carralero López (hcarralero@gmail.com)
 * @version 1.0
 */
public class TOLCode extends TOLLObject {

    private String function;

    /**
     * Obtiene el nombre de la función asignada al objeto.
     *
     * @return el nombre de la función asignada al objeto.
     */
    public String getFunction() {
        return function;
    }

    /**
     * Cambia el nombre de la función asignada al objeto.
     *
     * @param function nuevo nombre.
     */
    public void setFunction(String function) {
        this.function = function;
    }

    private String info;

    /**
     * Obtiene la información del objeto.
     *
     * @return la información del objeto.
     */
    public String getInfo() {
        return info;
    }

    /**
     * Construye un objeto de tipo <code>TOLCode</code>.
     *
     * @param function nombre de la función asignada al objeto.
     * @param info información.
     */
    public TOLCode(String function, String info) {
        this.function = function;
        this.info = info;
    }

    /**
     * Determina si el objeto corresponde al tipo Code de TOL.
     *
     * @return <code>true</code> ya que el objeto corresponde al tipo Code de TOL.
     */
    @Override
    public boolean isCode() {
        return true;
    }

    /**
     * Compara a este objeto con otro.
     *
     * @param obj el objeto con el que se comparará.
     * @return <code>true</code> si el objeto pasado como parámetro es de tipo <code>TOLCode</code>
     * y tiene nombre e información idéntica.
     */
    @Override
    public boolean equals(Object obj) {
        if (obj == null) {
            return false;
        }
        if (getClass() != obj.getClass()) {
            return false;
        }
        final TOLCode other = (TOLCode) obj;
        if ((this.function == null) ? (other.function != null) : !this.function.equals(other.function)) {
            return false;
        }
        if ((this.info == null) ? (other.info != null) : !this.info.equals(other.info)) {
            return false;
        }
        return true;
    }

    /**
     * Calcula el código hash asignado al objeto.
     *
     * @return el código hash asignado al objeto.
     */
    @Override
    public int hashCode() {
        int hash = 7;
        hash = 41 * hash + (this.function != null ? this.function.hashCode() : 0);
        hash = 41 * hash + (this.info != null ? this.info.hashCode() : 0);
        return hash;
    }

    /**
     * Obtiene una representación como cadena de texto del objeto en el formato <function>(<info>).
     *
     * @return la representación como cadena de texto del objeto en el formato <function>(<info>).
     */
    @Override
    public String toString() {
        String res="";
        if (this.info!=null&&this.info.length()>0) {
            res+= this.info;
        } else {
            res+= "<null>";
        }
        return res;
    }

}
