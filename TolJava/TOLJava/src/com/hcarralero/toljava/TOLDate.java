package com.hcarralero.toljava;

import java.text.DateFormat;
import java.util.Calendar;
import java.util.Date;

/**
 * Representa a un objeto TOL de tipo Date.
 *
 * @author Humberto Andrés Carralero López (hcarralero@gmail.com)
 * @version 1.0
 */
public class TOLDate extends TOLLObject {

    /**
     * Constante que representa la fecha del "Inicio del Tiempo"
     */
    public static final TOLDate BEGIN_DATE= new TOLDate('B');

    /**
     * Constante que representa la fecha del "Fin del Tiempo"
     */
    public static final TOLDate END_DATE= new TOLDate('E');

    /**
     * Construye un objeto <code>TOLDate</code> sin especificar valores. Útil para construir fechas especiales
     * no asociadas a fechas reales, tales como fechas TOL: el "INICIO" y el "FIN" de los tiempos
     */
    private TOLDate(char t) {
        switch (t) {
            case 'B':
                this.year= java.lang.Integer.MIN_VALUE;
                this.month= java.lang.Integer.MIN_VALUE;
                this.day= java.lang.Integer.MIN_VALUE;
                this.hour= java.lang.Integer.MIN_VALUE;
                this.minute= java.lang.Integer.MIN_VALUE;
                this.second= java.lang.Double.MIN_VALUE;
                break;
            case 'E':
                this.year= java.lang.Integer.MAX_VALUE;
                this.month= java.lang.Integer.MAX_VALUE;
                this.day= java.lang.Integer.MAX_VALUE;
                this.hour= java.lang.Integer.MAX_VALUE;
                this.minute= java.lang.Integer.MAX_VALUE;
                this.second= java.lang.Double.MAX_VALUE;
                break;
        }
    }

    /**
     * Construye un objeto <code>TOLDate</code> a partir de: año, mes, día, hora, minuto y segundo
     *
     * @param year      Año
     * @param month     Mes
     * @param day       Día
     * @param hour      Hora
     * @param minute    Minuto
     * @param second    Segundo
     * @throws TOLDateException cuando la combinación year-month-day-hour-minute-second no corresponde a una fecha válida
     */
    public TOLDate(int year, int month, int day, int hour, int minute, double second) throws TOLDateException {
        if (year<0) {
            throw new TOLDateException(TOLDateException.ERROR_INVALID_YEAR, "Año NO VÁLIDO por ser negativo");
        }
        this.year = year;
        if (month<1 ||month>12) {
            throw new TOLDateException(TOLDateException.ERROR_INVALID_MONTH, "Mes NO VÁLIDO por no estar en el rango 1-12");
        }
        this.month = month;
        int nd= 30;
        if (month==1 || month==3 || month==5 || month==7 || month==8 || month==10 || month==12) {
            nd= 31;
        }
        if (month==2) {
            if ((year % 4 == 0 && year % 100 != 0) || year % 400 == 0) {
                //El año es bisiesto
                nd= 29;
            } else {
                nd= 28;
            }
        }
        if (day<1 ||day>nd) {
            throw new TOLDateException(TOLDateException.ERROR_INVALID_DAY, "Día NO VÁLIDO por no estar en el rango de valores permitidos para el mes");
        }
        this.day = day;
        if (hour<0 ||hour>23) {
            throw new TOLDateException(TOLDateException.ERROR_INVALID_HOUR, "Hora NO VÁLIDA por no estar en el rango 0-23");
        }
        this.hour = hour;
        if (minute<0 ||minute>59) {
            throw new TOLDateException(TOLDateException.ERROR_INVALID_MINUTE, "Minuto NO VÁLIDO por no estar en el rango 0-59");
        }
        this.minute = minute;
        if (second<0 ||second>59) {
            throw new TOLDateException(TOLDateException.ERROR_INVALID_MINUTE, "Minuto NO VÁLIDO por no estar en el rango 0-59");
        }
        this.second = second;
    }

    /**
     * Construye un objeto <code>TOLDate</code> a partir de: año, mes y día.
     *
     * @param year      Año
     * @param month     Mes
     * @param day       Día
     * @throws TOLDateException cuando la combinación year-month-day no corresponde a una fecha válida
     */
    public TOLDate(int year, int month, int day) throws TOLDateException {
        if (year<0) {
            throw new TOLDateException(TOLDateException.ERROR_INVALID_YEAR, "Año NO VÁLIDO por ser negativo");
        }
        this.year = year;
        if (month<1 ||month>12) {
            throw new TOLDateException(TOLDateException.ERROR_INVALID_MONTH, "Mes NO VÁLIDO por no estar en el rango 1-12");
        }
        this.month = month;
        int nd= 30;
        if (month==1 || month==3 || month==5 || month==7 || month==8 || month==10 || month==12) {
            nd= 31;
        }
        if (month==2) {
            if ((year % 4 == 0 && year % 100 != 0) || year % 400 == 0) {
                //El año es bisiesto
                nd= 29;
            } else {
                nd= 28;
            }
        }
        if (day<1 ||day>nd) {
            throw new TOLDateException(TOLDateException.ERROR_INVALID_DAY, "Día NO VÁLIDO por no estar en el rango de valores permitidos para el mes");
        }
        this.day = day;
        this.hour= 0;
        this.minute= 0;
        this.second= 0;
    }

    private int year;

    /**
     * Obtiene el año correspondiente a la fecha almacenada
     *
     * @return el año
     */
    public int getYear() {
        return year;
    }

    /**
     * Cambia el año por el valor indicado en el parámetro.
     *
     * @param year nuevo valor para el año.
     * @throws TOLDateException cuando el año no corresponde a una fecha válida
     */
    public void setYear(int year) throws TOLDateException {
        if (year<0) {
            throw new TOLDateException(TOLDateException.ERROR_INVALID_YEAR, "Año NO VÁLIDO por ser negativo");
        }
        this.year = year;
    }
    
    private int month;

    /**
     * Obtiene el mes correspondiente a la fecha almacenada
     *
     * @return el mes
     */
    public int getMonth() {
        return month;
    }

    /**
     * Cambia el valor del mes por el valor indicado en el parámetro.
     *
     * @param month nuevo valor para el mes.
     * @throws TOLDateException cuando el mes no corresponde a una fecha válida
     */
    public void setMonth(int month) throws TOLDateException {
        if (month<1 ||month>12) {
            throw new TOLDateException(TOLDateException.ERROR_INVALID_MONTH, "Mes NO VÁLIDO por no estar en el rango 1-12");
        }
        this.month = month;
    }

    private int day;

    /**
     * Obtiene el día correspondiente a la fecha almacenada
     *
     * @return el día.
     */
    public int getDay() {
        return day;
    }

    /**
     * Cambia el valor del día por el valor indicado en el parámetro.
     *
     * @param day nuevo valor para el día.
     * @throws TOLDateException cuando el día no corresponde a una fecha válida
     */
    public void setDay(int day) throws TOLDateException {
        int nd= 30;
        if (this.month==1 || this.month==3 || this.month==5 || this.month==7 || this.month==8 || this.month==10 || this.month==12) {
            nd= 31;
        }
        if (this.month==2) {
            if ((this.year % 4 == 0 && this.year % 100 != 0) || this.year % 400 == 0) {
                //El año es bisiesto
                nd= 29;
            } else {
                nd= 28;
            }
        }
        if (day<1 ||day>nd) {
            throw new TOLDateException(TOLDateException.ERROR_INVALID_DAY, "Día NO VÁLIDO por no estar en el rango de valores permitidos para el mes");
        }
        this.day = day;
    }

    private int hour= 0;

    /**
     * Obtiene la hora correspondiente a la fecha almacenada
     *
     * @return la hora
     */
    public int getHour() {
        return hour;
    }

    /**
     * Cambia el valor de la hora por el valor indicado en el parámetro
     *
     * @param hour nuevo valor para la hora
     * @throws TOLDateException cuando la hora no corresponde a una fecha válida
     */
    public void setHour(int hour) throws TOLDateException {
        if (hour<0 ||hour>23) {
            throw new TOLDateException(TOLDateException.ERROR_INVALID_HOUR, "Hora NO VÁLIDA por no estar en el rango 0-23");
        }
        this.hour = hour;
    }

    private int minute= 0;

    /**
     * Obtiene el minuto correspondiente a la fecha almacenada
     *
     * @return el minuto
     */
    public int getMinute() {
        return minute;
    }

    /**
     * Cambia el valor del minuto por el valor indicado en el parámetro.
     *
     * @param minute nuevo valor para el minuto
     * @throws TOLDateException cuando el minuto no corresponde a una fecha válida
     */
    public void setMinute(int minute) throws TOLDateException {
        if (minute<0 ||minute>59) {
            throw new TOLDateException(TOLDateException.ERROR_INVALID_MINUTE, "Minuto NO VÁLIDO por no estar en el rango 0-59");
        }
        this.minute = minute;
    }

    private double second= 0.0;

    /**
     * Obtiene el segundo correspondiente a la fecha almacenada
     *
     * @return el segundo
     */
    public double getSecond() {
        return second;
    }

    /**
     * Cambia el valor del segundo por el valor indicado en el parámetro
     *
     * @param second nuevo valor para el segundo
     * @throws TOLDateException cuando el segundo no corresponde a una fecha válida
     */
    public void setSecond(double second) throws TOLDateException {
        if (second<0 ||second>59) {
            throw new TOLDateException(TOLDateException.ERROR_INVALID_MINUTE, "Minuto NO VÁLIDO por no estar en el rango 0-59");
        }
        this.second = second;
    }

    /**
     * Cambia el valor de la fecha por el valor indicado en el parámetro
     *
     * @param date la nueva fecha
     */
    public void setDate(Date date)     {
        Calendar cal= Calendar.getInstance();
        cal.setTime(date);
        this.year= cal.get(Calendar.YEAR);
        this.month= cal.get(Calendar.MONTH)-1;
        this.day= cal.get(Calendar.DAY_OF_MONTH);
        this.hour= cal.get(Calendar.HOUR_OF_DAY);
        this.minute= cal.get(Calendar.MINUTE);
        this.second= cal.get(Calendar.SECOND);
    }

    /**
     * Obtiene como un objeto <code>java.util.Date</code> la fecha almacenada
     *
     * @return la fecha almacenada como un objeto <code>java.util.Date</code>
     */
    public Date getDate() {
        Calendar cal= Calendar.getInstance();
        cal.set(year, month+1, day+1, hour, minute, (int)second);
        Date date = cal.getTime();
        return date;
    }
    
    /**
     * Determina si el objeto corresponde al tipo Date de TOL
     *
     * @return <code>true</code> ya que el objeto corresponde al tipo Date de TOL
     */
    @Override
    public boolean isDate() {
        return true;
    }

    /**
     * Compara a este objeto con otro
     *
     * @param obj el objeto con el que se comparará
     * @return <code>true</code> si el objeto pasado como parámetro es de tipo <code>TOLDate</code> y contiene una fecha idéntica
     */
    @Override
    public boolean equals(Object obj) {
        if (obj == null) {
            return false;
        }
        if (getClass() != obj.getClass()) {
            return false;
        }
        final TOLDate other = (TOLDate) obj;
        if (this.year != other.year || this.month!=other.month || this.day!=other.day || this.hour!=other.hour || this.minute!=other.minute || this.second!=other.second) {
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
        int hash = 3;
        hash = 31 * hash + this.year;
        hash = 31 * hash + this.month;
        hash = 31 * hash + this.day;
        hash = 31 * hash + this.hour;
        hash = 31 * hash + this.minute;
        hash = 31 * hash + (int) (Double.doubleToLongBits(this.second) ^ (Double.doubleToLongBits(this.second) >>> 32));
        return hash;
    }

    /**
     * Obtiene una representación como cadena de texto de la fecha
     *
     * @return la representación como cadena de texto de la fecha
     */
    @Override
    public String toString() {
        Calendar cal= Calendar.getInstance();
        cal.set(year, month-1, day, hour, minute, (int)second);
        Date date = cal.getTime();
        DateFormat df= DateFormat.getDateTimeInstance();
        return df.format(date);
    }

}
