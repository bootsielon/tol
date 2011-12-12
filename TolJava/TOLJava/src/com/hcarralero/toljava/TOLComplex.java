package com.hcarralero.toljava;

/**
 * Representa a un objeto TOL de tipo Complex
 *
 * @author Humberto Andrés Carralero López (hcarralero@gmail.com)
 * @version 1.0
 */
public class TOLComplex extends TOLLObject {

    /**
     * Construye un objeto de tipo <code>TOLComplex</code>
     *
     * @param x parte real del número complejo
     * @param y parte imaginaria del número complejo
     */
    public TOLComplex(double x, double y) {
        this.x = x;
        this.y = y;
    }

    private double x;

    /**
     * Obtiene el valor de x (parte real del número complejo)
     *
     * @return el valor de x (parte real del número complejo)
     */
    public double getX() {
        return x;
    }

    /**
     * Cambia el valor de x (parte real) por el valor pasado como parámetro
     *
     * @param x nuevo valor de x (parte real)
     */
    public void setX(double x) {
        this.x = x;
    }
    private double y;

    /**
     * Obtiene el valor de y (parte imaginaria) del número complejo)
     *
     * @return el valor de y (parte imaginaria)
     */
    public double getY() {
        return y;
    }

    /**
     * Cambia el valor de y (parte imaginaria) por el valor pasado como parámetro
     *
     * @param y nuevo valor de y (parte imaginaria)
     */
    public void setY(double y) {
        this.y = y;
    }

    /**
     * Determina si el objeto corresponde al tipo Complex de TOL
     *
     * @return <code>true</code> ya que el objeto corresponde al tipo Complex de TOL
     */
    @Override
    public boolean isComplex() {
        return true;
    }

    /**
     * Compara a este objeto con otro
     *
     * @param obj el objeto con el que se comparará
     * @return <code>true</code> si el objeto pasado como parámetro es de tipo <code>TOLReal</code> y contiene un valor numérico real idéntico
     */
    @Override
    public boolean equals(Object obj) {
        if (obj.getClass()!=TOLComplex.class) {
            return false;
        }
        TOLComplex other= (TOLComplex)obj;
        if (x!=other.x||y!=other.y) {
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
        hash = 47 * hash + (int) (Double.doubleToLongBits(this.x) ^ (Double.doubleToLongBits(this.x) >>> 32));
        hash = 47 * hash + (int) (Double.doubleToLongBits(this.y) ^ (Double.doubleToLongBits(this.y) >>> 32));
        return hash;
    }

    /**
     * Obtiene una representación como cadena de texto del número complejo en el formato "x+yi"
     *
     * @return la representación como cadena de texto del número complejo en el formato "x+yi"
     */
    @Override
    public String toString() {
        return ""+x+"+"+y+"i";
    }

}
