
package com.hcarralero.toljava;

/**
 * Representa a una variable de TOL, consta de nombre, descripción y valor
 *
 * @author Humberto Andrés Carralero López (hcarralero@gmail.com)
 * @version 1.0
 */
public class TOLVariable {

    /**
     * Construye un objeto <code>TOLVariable</code>
     *
     * @param name nombre
     * @param value valor
     * @param desc descripción
     */
    public TOLVariable(String name, TOLObject value, String desc) {
        this.name = name;
        this.value = value;
        this.desc = desc;
    }

    private String name = "NO_NAME";

    /**
     * Retorna el nombre de la variable
     *
     * @return el nombre de la variable
     */
    public String getName() {
        return name;
    }

    /**
     * Cambia el nombre de la variable por el nuevo nombre
     *
     * @param name el nuevo nombre.
     */
    public void setName(String name) {
        this.name = name;
    }

    private TOLObject value= null;

    /**
     * Retorna el valor de la variable
     *
     * @return el valor de la variable
     */
    public TOLObject getValue() {
        return value;
    }

    /**
     * Cambia el valor de la variable por el nuevo valor
     *
     * @param value el nuevo valor
     */
    public void setValue(TOLObject value) {
        this.value = value;
    }

    private String desc= "Sin descripción";

    /**
     * Retorna la descripción de la variable
     *
     * @return la descripción de la variable
     */
    public String getDesc() {
        return desc;
    }

    /**
     * Cambia la descripción de la variable por la nueva descripción
     *
     * @param desc la nueva descripción
     */
    public void setDesc(String desc) {
        this.desc = desc;
    }

    /**
     * Compara a este objeto con otro
     *
     * @param obj el objeto con el que se comparará
     * @return <code>true</code> si el objeto pasado como parámetroves de tipo
     * <code>TOLVariable</code>, tiene el mismo nombre, la misma descripción y el
     * mismo valor
     */
    @Override
    public boolean equals(Object obj) {
        if (obj == null) {
            return false;
        }
        if (getClass() != obj.getClass()) {
            return false;
        }
        final TOLVariable other = (TOLVariable) obj;
        if ((this.name == null) ? (other.name != null) : !this.name.equals(other.name)) {
            return false;
        }
        if (this.value != other.value && (this.value == null || !this.value.equals(other.value))) {
            return false;
        }
        if ((this.desc == null) ? (other.desc != null) : !this.desc.equals(other.desc)) {
            return false;
        }
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
        hash = 37 * hash + (this.name != null ? this.name.hashCode() : 0);
        hash = 37 * hash + (this.value != null ? this.value.hashCode() : 0);
        hash = 37 * hash + (this.desc != null ? this.desc.hashCode() : 0);
        return hash;
    }

    /**
     * Obtiene una representación como cadena de texto de la variable en el formato
     * [name: <nombre>], [desc: <descripción>], [value: <value>]
     *
     * @return la representación como cadena de texto de la variable
     */
    @Override
    public String toString() {
        return "[name: "+this.name+"], [desc: "+this.desc+"], [value: "+this.value.toString()+"]";
    }

}
