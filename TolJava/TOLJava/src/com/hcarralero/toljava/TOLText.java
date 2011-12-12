
package com.hcarralero.toljava;

import java.io.Serializable;

/**
 * Representa a un objeto TOL de tipo Text
 *
 * @author Humberto Andrés Carralero López (hcarralero@gmail.com)
 * @version 1.0
 */
public class TOLText extends TOLLObject implements Serializable {

    private String text= null;

    /**
     * Construye un objeto de tipo TOLText
     *
     * @param text Texto que almacena el objeto
     */
    public TOLText(String text) {
        this.text = text;
    }

    /**
     * Retorna el texto almacenado
     *
     * @return el texto almacenado
     */
    public String getText() {
        return text;
    }

    /**
     * Cambia el texto almacenado por el nuevo texto
     *
     * @param text el nuevo texto
     */
    public void setText(String text) {
        this.text = text;
    }

    /**
     * Determina si el objeto corresponde al tipo Text de TOL
     *
     * @return <code>true</code> ya que el objeto corresponde al tipo Text de TOL
     */
    @Override
    public boolean isText() {
        return true;
    }

    /**
     * Compara a este objeto con otro
     *
     * @param obj el objeto con el que se comparará
     * @return <code>true</code> si el objeto pasado como parámetro es de tipo
     * <code>TOLText</code> y contiene un texto similar
     */
    @Override
    public boolean equals(Object obj) {
        if (obj == null) {
            return false;
        }
        if (getClass() != obj.getClass()) {
            return false;
        }
        final TOLText other = (TOLText) obj;
        if ((this.text == null) ? (other.text != null) : !this.text.equals(other.text)) {
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
        hash = 97 * hash + (this.text != null ? this.text.hashCode() : 0);
        return hash;
    }

    /**
     * Obtiene una representación como cadena de texto del objeto
     *
     * @return la representación como cadena de texto del objeto
     */
    @Override
    public String toString() {
        return this.text;
    }

}
