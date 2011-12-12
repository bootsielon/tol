package com.hcarralero.toljava;

import java.util.Iterator;
import java.util.Vector;

/**
 * Representa a un objeto TOL de tipo Polyn.
 *
 * @author Humberto Andrés Carralero López (hcarralero@gmail.com)
 * @version 1.0
 */
public class TOLPolyn extends TOLLObject {

    /**
     * Variable B de un término.
     */
    public static int B= 0;

    /**
     * Variable F de un término.
     */
    public static int F= 1;

    private Vector<TOLPolynTerm> vterms;

    /**
     * Obtiene una copia exacta de todos los términos del polinomio.
     *
     * @return un arreglo con una copia de todos los términos del polinomio.
     */
    public TOLPolynTerm[] getTerms() {
        TOLPolynTerm[] terms= new TOLPolynTerm[vterms.size()];
        return vterms.toArray(terms);
    }

    /**
     * Construye un polinomio sin términos, para añadir términos debe usar el método
     * addTerm.
     */
    public TOLPolyn() {
        vterms= new Vector<TOLPolynTerm>();
    }

    /**
     * Añade un nuevo término al polinomio.
     *
     * @param var variable del término (B o F).
     * @param coef coeficiente del término.
     * @param grade grado del término.
     */
    public synchronized void addTerm(int var, double coef, int grade) {
        if (coef!=0) {
            if (grade<0) {
                grade= -grade;
                if (var==TOLPolyn.B) {
                    var= TOLPolyn.F;
                } else {
                    var= TOLPolyn.B;
                }
            }
            int pos= 0;
            Iterator<TOLPolynTerm> itp= vterms.iterator();
            while (itp.hasNext()) {
                TOLPolynTerm t= itp.next();
                if (t.var==TOLPolyn.B) {
                    if (var==TOLPolyn.B) {
                        if (t.grade==grade) {
                            t.coef+= coef;
                            if (t.coef==0) {
                                itp.remove();
                            }
                            pos= -1;
                            break;
                        } else {
                            if (t.grade>grade) {
                                vterms.add(pos, new TOLPolynTerm(var, coef, grade));
                                pos= -1;
                                break;
                            }
                        }

                    } else {
                        vterms.add(pos, new TOLPolynTerm(var, coef, grade));
                        pos= -1;
                        break;
                    }
                } else {
                    if (var==TOLPolyn.F) {
                        if (t.grade==grade) {
                            t.coef+= coef;
                            pos= -1;
                            break;
                        } else {
                            if (grade>t.grade) {
                                vterms.add(pos, new TOLPolynTerm(var, coef, grade));
                                pos= -1;
                                break;
                            }
                        }

                    } else {
                        vterms.add(pos, new TOLPolynTerm(var, coef, grade));
                        pos= -1;
                        break;
                    }
                }
                ++pos;
            }
            if (pos!=-1) {
                vterms.add(pos, new TOLPolynTerm(var, coef, grade));
            }
        }
    }

    /**
     * Elimina el término de la posición <code>index</code> del polinomio.
     *
     * @param index posición del téremino en el polinomio.
     * @return una copia del término eliminado del polinomio.
     */
    public synchronized TOLPolynTerm removeTerm(int index) {
        return vterms.remove(index);
    }

    /**
     * Obtiene el número de términos del polinomio.
     *
     * @return el número de términos del polinomio.
     */
    public synchronized int getNterms() {
        return vterms.size();
    }

    /**
     * Obtiene el término ubicado en la posición <code>index</code> del polinomio.
     *
     * @param index posición del término dentro del polinomio.
     * @return el término ubicado en la posición <code>index</code> del polinomio.
     */
    public synchronized TOLPolynTerm getTerm(int index) {
        return vterms.get(index);
    }

    /**
     * Obtiene un iterador que puede ser usado para moverse entre los términos del polinomio.
     *
     * @return el iterador sobre los términos.
     */
    public Iterator<TOLPolynTerm> getIterator() {
        return vterms.iterator();
    }

    /**
     * Determina si el objeto corresponde al tipo Polyn de TOL.
     *
     * @return <code>true</code> ya que el objeto corresponde al tipo Polyn de TOL.
     */
    @Override
    public boolean isPolyn() {
        return true;
    }

    /**
     * Compara a este objeto con otro.
     *
     * @param obj el objeto con el que se comparará.
     * @return <code>true</code> si el objeto pasado como parámetro es de tipo <code>TOLPolyn</code>
     * y contiene los mismos términos.
     */
    @Override
    public boolean equals(Object obj) {
        if (obj == null) {
            return false;
        }
        if (getClass() != obj.getClass()) {
            return false;
        }
        final TOLPolyn other = (TOLPolyn) obj;
        if (this.vterms != other.vterms && (this.vterms == null || !this.vterms.equals(other.vterms))) {
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
        hash = 47 * hash + (this.vterms != null ? this.vterms.hashCode() : 0);
        return hash;
    }

    /**
     * Obtiene una representación como cadena de texto del polinomio usando la sintaxis de TOL.
     *
     * @return la representación como cadena de texto del polinomio.
     */
    @Override
    public String toString() {
        String res= "";
        boolean mas= false;
        Iterator<TOLPolynTerm> it= this.vterms.iterator();
        while (it.hasNext()) {
            TOLPolynTerm t= it.next();
            if (mas) {
                if (t.coef>=0) {
                    res+= "+";
                }
            } else {
                mas= true;
            }
            res+= t.toString();
        }
        return res;
    }

}
