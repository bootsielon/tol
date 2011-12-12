
package com.hcarralero.toljava;

import java.util.Arrays;

/**
 * Representa a un objeto TOL de tipo VMatrix
 *
 * @author Humberto Andrés Carralero López (hcarralero@gmail.com)
 * @version 1.0
 * @see com.hcarralero.toljava.TOLMatrixException
 */
public class TOLVMatrix extends TOLLObject {

    /**
     * Construye un objeto <code>TOLMatrix</code> vacío (UnknownMatrix)
     *
     */
    public TOLVMatrix() {
        this.rows= 0;
        this.columns= 0;
        this.values= null;
    }

    /**
     * Construye un objeto <code>TOLVMatrix</code> con el número especificado de filas y columnas,
     * todos los elementos son inicializados a <code>0.0</code>
     *
     * @param rows número de filas
     * @param columns número de columnas
     * @throws com.hcarralero.toljava.TOLMatrixException cuando no se puede crear la matriz por alguna de
     * las razones siguientes: número de filas o columnas no válidos o insuficicnte memoria
     * @see com.hcarralero.toljava.TOLMatrixException
     */
    public TOLVMatrix(int rows, int columns) throws TOLMatrixException {
        if (rows<=0) {
            throw new TOLMatrixException(TOLMatrixException.ERROR_INVALID_ROWS, "Número de filas de la matriz NO ES VÁLIDO (<=0)");
        }
        this.rows = rows;
        if (columns<=0) {
            throw new TOLMatrixException(TOLMatrixException.ERROR_INVALID_COLUMNS, "Número de columnas de la matriz NO ES VÁLIDO (<=0)");
        }
        this.columns = columns;
        this.values= new double[rows][];
        if (values==null) {
            throw new TOLMatrixException(TOLMatrixException.ERROR_NOT_ENOUGH_MEMORY, "No hay memoria suficiente para crear la matriz");
        }
        for (int i= 0; i<rows; ++i) {
            values[i]= new double[columns];
            if (values[i]==null) {
                throw new TOLMatrixException(TOLMatrixException.ERROR_NOT_ENOUGH_MEMORY, "No hay memoria suficiente para crear la matriz");
            }
            for (int j= 0; j<columns; ++j) {
                values[i][j]= 0.0;
            }
        }
    }

    /**
     * Construye un objeto <code>TOLVMatrix</code>na partie de un arreglo doble (matriz) de números reales
     *
     * @param values arreglo doble (matriz) de números reales
     * @throws com.hcarralero.toljava.TOLMatrixException cuando no se puede crear la matriz por alguna de
     * las razones siguientes: número de filas o columnas no válidos en el arreglo doble (matriz) pasado como parámetro
     * o insuficicnte memoria
     * @see com.hcarralero.toljava.TOLMatrixException
     */
    public TOLVMatrix(double values[][]) throws TOLMatrixException {
        this.rows = values.length;
        if (rows<=0) {
            throw new TOLMatrixException(TOLMatrixException.ERROR_INVALID_ROWS, "Número de filas de la matriz NO ES VÁLIDO (<=0)");
        }
        this.columns= values[0].length;
        if (this.columns<=0) {
            throw new TOLMatrixException(TOLMatrixException.ERROR_INVALID_COLUMNS, "Número de columnas de la matriz NO ES VÁLIDO (<=0)");
        }
        this.values= new double[this.rows][];
        if (this.values==null) {
            throw new TOLMatrixException(TOLMatrixException.ERROR_NOT_ENOUGH_MEMORY, "No hay memoria suficiente para crear la matriz");
        }
        for (int i= 0; i<this.rows; ++i) {
            if (values[i].length!=this.columns) {
                throw new TOLMatrixException(TOLMatrixException.ERROR_NOT_EQUALS_ROWS, "No todas las filas tienen igual cantidad de columnas");
            }
            this.values[i]= new double[this.columns];
            if (this.values[i]==null) {
                throw new TOLMatrixException(TOLMatrixException.ERROR_NOT_ENOUGH_MEMORY, "No hay memoria suficiente para crear la matriz");
            }
            for (int j= 0; j<this.columns; ++j) {
                this.values[i][j]= values[i][j];
            }
        }
    }

    private int rows= 0;

    /**
     * Obtiene el número de filas de la matriz
     *
     * @return el número de filas de la matriz
     */
    public int getRows() {
        return rows;
    }

    private int columns= 0;

    /**
     * Obtiene el número de columnas de la matriz
     *
     * @return el número de columnas de la matriz
     */
    public int getColumns() {
        return columns;
    }

    protected double[][] values = null;

    /**
     * Obtiene como arreglo doble la matriz contenida en el objeto
     *
     * @return arreglo doble la con matriz contenida
     */
    public double[][] getValues() {
        return values;
    }

    /**
     * Obtiene el valor numérico real correspondiente a la celda cuyas fila y columna se indican
     *
     * @param row fila
     * @param column columna
     * 
     * @return el valor numérico real correspondiente a la celda indicada
     *
     * @throws com.hcarralero.toljava.TOLMatrixException si se intenta acceder a una celda inexistente
     */
    public double getCellValue(int row, int column) throws TOLMatrixException {
        if (row<0 || row>=this.rows) {
            throw new TOLMatrixException(TOLMatrixException.ERROR_INVALID_ROW_NUMBER, "Se intenta acceder a un NÚMERO DE FILA NO VÁLIDO (<0 o >número de filas de la matriz)");
        }
        if (column<0 || column>=this.columns) {
            throw new TOLMatrixException(TOLMatrixException.ERROR_INVALID_COLUMN_NUMBER, "Se intenta acceder a un NÚMERO DE COLUMNA NO VÁLIDO (<0 o >número de columnas de la matriz)");
        }
        return this.values[row][column];
    }

    /**
     * Cambia el valor de la celda cuyas fila y columna se indican por el nuevo valor pasado como parámetro
     *
     * @param row fila
     * @param column columna
     * @param value nuevo valor
     * @throws com.hcarralero.toljava.TOLMatrixException cuando la fila o columna están fuera del rango de la matriz
     */
    public void setCellValue(int row, int column, double value) throws TOLMatrixException {
        if (row<0 || row>=this.rows) {
            throw new TOLMatrixException(TOLMatrixException.ERROR_INVALID_ROW_NUMBER, "Se intenta acceder a un NÚMERO DE FILA NO VÁLIDO (<0 o >número de filas de la matriz)");
        }
        if (column<0 || column>=this.columns) {
            throw new TOLMatrixException(TOLMatrixException.ERROR_INVALID_COLUMN_NUMBER, "Se intenta acceder a un NÚMERO DE COLUMNA NO VÁLIDO (<0 o >número de columnas de la matriz)");
        }
        this.values[row][column]= value;
    }

    /**
     * Determina si el objeto corresponde al tipo VMatrix de TOL
     *
     * @return <code>true</code> ya que el objeto corresponde al tipo VMatrix de TOL
     */
    @Override
    public boolean isVMatrix() {
        return true;
    }
    /**
     * Compara a este objeto con otro
     *
     * @param obj el objeto con el que se comparará
     * @return <code>true</code> si el objeto pasado como parámetro es de tipo
     * <code>TOLVMatrix</code> y representa una matriz con igual cantidad de filas
     * y columnas, así como todos los elementos que contienen
     */
    @Override
    public boolean equals(Object obj) {
        if (obj == null) {
            return false;
        }
        if (getClass() != obj.getClass()) {
            return false;
        }
        final TOLVMatrix other = (TOLVMatrix) obj;
        if (this.rows != other.rows) {
            return false;
        }
        if (this.columns != other.columns) {
            return false;
        }
        if (this.values != other.values && (this.values == null || !Arrays.deepEquals(this.values, other.values))) {
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
        hash = 89 * hash + this.rows;
        hash = 89 * hash + this.columns;
        hash = 89 * hash + (this.values != null ? this.values.hashCode() : 0);
        return hash;
    }

    /**
     * Obtiene una representación como cadena de texto de la matriz en el formato
     * {{a11, a12, ..., a1n}, {a21, a22, ..., a2n}, ..., {am1, am2, ..., amn}}, donde
     * m es el número de filas y m el de columnas
     *
     * @return la representación como cadena de texto de la matriz
     */
    @Override
    public String toString() {
        if (this.rows<=0||this.columns<=0) {
            return "UnknownVMatrix";
        }
        String res= "{";
        for (int i= 0; i<this.rows; ++i) {
            res+= "{";
            boolean coma= false;
            for (int j= 0; j<this.columns; ++j) {
                if (coma) {
                    res+= ", ";
                } else {
                    coma= true;
                }
                res+= this.values[i][j];
            }
            res+= "}";
        }
        res+= "}";
        return res;
    }

}
