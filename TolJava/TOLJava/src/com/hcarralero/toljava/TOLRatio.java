package com.hcarralero.toljava;

/**
 * Representa a un objeto TOL de tipo Ratio.
 *
 * @author Humberto Andrés Carralero López (hcarralero@gmail.com)
 * @version 1.0
 */
public class TOLRatio extends TOLLObject {

    private TOLPolyn numerator;

    /**
     * Obtiene el polinomio que hace de numerador
     *
     * @return el polinomio numerador
     */
    public TOLPolyn getNumerator() {
        return numerator;
    }

    /**
     * Establece el polinomio que hace de numerador
     *
     * @param numerator el polinomio numerador
     */
    public void setNumerator(TOLPolyn numerator) {
        this.numerator = numerator;
    }

    private TOLPolyn denominator;

    /**
     * Obtiene el polinomio que hace de denominador
     *
     * @return el polinomio denominador
     */
    public TOLPolyn getDenominator() {
        return denominator;
    }

    /**
     * Establece el polinomio que hace de denominador
     *
     * @param denominator el polinomio denominador
     */
    public void setDenominator(TOLPolyn denominator) {
        this.denominator = denominator;
    }

    /**
     * Construye un objeto <code>TOLRatio</code> a partir de dos polinomios, el primero
     * hará la función de numerador y el segundo la de denominador
     *
     * @param numerator polinomio numerador
     * @param denominator oilinomio denominador
     */
    public TOLRatio(TOLPolyn numerator, TOLPolyn denominator) {
        this.numerator = numerator;
        this.denominator = denominator;
    }

    /**
     * Determina si el objeto corresponde al tipo Ratio de TOL.
     *
     * @return <code>true</code> ya que el objeto corresponde al tipo Ratio de TOL.
     */
    @Override
    public boolean isRatio() {
        return true;
    }

    /**
     * Compara a este objeto con otro
     *
     * @param obj el objeto con el que se comparará
     * @return <code>true</code> si el objeto pasado como parámetro es de tipo
     * <code>TOLRatio</code> y ambos tienen idénticos polinomios como numerador y denominador
     */
    @Override
    public boolean equals(Object obj) {
        if (obj == null) {
            return false;
        }
        if (getClass() != obj.getClass()) {
            return false;
        }
        final TOLRatio other = (TOLRatio) obj;
        if (this.numerator != other.numerator && (this.numerator == null || !this.numerator.equals(other.numerator))) {
            return false;
        }
        if (this.denominator != other.denominator && (this.denominator == null || !this.denominator.equals(other.denominator))) {
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
        int hash = 5;
        hash = 17 * hash + (this.numerator != null ? this.numerator.hashCode() : 0);
        hash = 17 * hash + (this.denominator != null ? this.denominator.hashCode() : 0);
        return hash;
    }

    /**
     * Obtiene una representación como cadena de texto del objeto tipo <code>TOLRatio</code>
     * en el formato (numerador)/(denominador)
     *
     * @return la representación como cadena de texto del objeto tipo <code>TOLRatio</code>
     */
    @Override
    public String toString() {
        String res= "";
        if (this.numerator==null||this.numerator.getNterms()==0) {
            res= "<<NULL>>";
        } else {
            if (this.denominator==null||this.denominator.getNterms()==0) {
                res= "<<INFINITO>>";
            } else {
                if (this.numerator.getNterms()>1) {
                    res+= "(";
                }
                res+= this.numerator.toString();
                if (this.numerator.getNterms()>1) {
                    res+= ")";
                }
                res+= "/";
                if (this.denominator.getNterms()>1) {
                    res+= "(";
                }
                res+= this.denominator.toString();
                if (this.denominator.getNterms()>1) {
                    res+= ")";
                }
            }
        }
        return res;
    }
}

