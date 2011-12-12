package com.hcarralero.toljava;

/**
 * Representa a un término en un Polyn de TOL.
 *
 * @author Humberto Andrés Carralero López (hcarralero@gmail.com)
 * @version 1.0
 */
public class TOLPolynTerm {

    /**
     * Tipo de variable del término (B/F).
     */
    protected int var;

    /**
     * Obtiene el tipo de variable del término.
     *
     * @return B(0) o F(1), según sea lo almacenado en el término.
     */
    public int getVar() {
        return var;
    }

    /**
     * Establece el tipo de variable del término.
     *
     * @param var el tipo de la variable (B o F).
     */
    public void setVar(int var) {
        if (var != 0) {
            this.var = 1;
        } else {
            this.var = 0;
        }
    }
    /**
     * Coeficiente del término.
     */
    protected double coef;

    /**
     * Obtiene el coeficiente del término.
     *
     * @return el coeficiente del término.
     */
    public double getCoef() {
        return coef;
    }

    /**
     * Establece el coeficiente del término.
     *
     * @param coef coeficiente del término.
     */
    public void setCoef(double coef) {
        this.coef = coef;
    }
    /**
     * Grado del término.
     */
    protected int grade;

    /**
     * Obtiene el grado del término.
     *
     * @return el grado del término.
     */
    public int getGrade() {
        return grade;
    }

    /**
     * Establece el grado del término.
     *
     * @param grade el grado del término.
     */
    public void setGrade(int grade) {
        this.grade = grade;
    }

    /**
     * Contruye un <code>TOLPolynTerm</code> (término de un polinomio) usando los datos pasados por parámetro.
     *
     * @param var tipo de variable (B/F) del término.
     * @param coef coeficiente del término.
     * @param grade grado del término.
     */
    public TOLPolynTerm(int var, double coef, int grade) {
        if (var == 0) {
            this.var = TOLPolyn.B;
        } else {
            this.var = TOLPolyn.F;
        }
        this.coef = coef;
        this.grade = grade;
    }

    /**
     * Compara a este objeto con otro.
     *
     * @param obj el objeto con el que se comparará.
     * @return <code>true</code> si el objeto pasado como parámetro es de tipo <code>TOLPolynTerm</code>
     * y tiene valores idénticos para variable, coeficiente y grado.
     */
    @Override
    public boolean equals(Object obj) {
        if (obj == null) {
            return false;
        }
        if (getClass() != obj.getClass()) {
            return false;
        }
        final TOLPolynTerm other = (TOLPolynTerm) obj;
        if (this.var != other.var) {
            return false;
        }
        if (this.coef != other.coef) {
            return false;
        }
        if (this.grade != other.grade) {
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
        int hash = 5;
        hash = 11 * hash + this.var;
        hash = 11 * hash + (int) (Double.doubleToLongBits(this.coef) ^ (Double.doubleToLongBits(this.coef) >>> 32));
        hash = 11 * hash + this.grade;
        return hash;
    }

    /**
     * Obtiene una representación como cadena de texto del término del polinomio
     * usando la sintaxis de TOL.
     *
     * @return la representación como cadena de texto del término de polinomio.
     */
    @Override
    public String toString() {
        String res = "";
        if (this.coef != 0) {
            if (this.coef == -1 && this.grade != 0) {
                res += "-";
            } else {
                if (this.coef != 1 || this.grade == 0) {
                    res += this.coef;
                }
            }
            if (this.grade > 0) {
                if (this.coef != -1 && this.coef != 1) {
                    res += "*";
                }
                if (this.var == TOLPolyn.B) {
                    res += "B";
                } else {
                    res += "F";
                }
                if (this.grade > 1) {
                    res += "^" + grade;
                }
            }
        }
        return res;
    }
}
