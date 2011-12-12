
package com.hcarralero.toljava.demo;

import com.hcarralero.toljava.TOLMachine;
import com.hcarralero.toljava.TOLVariable;
import com.sun.rave.web.ui.appbase.AbstractSessionBean;
import com.sun.webui.jsf.model.Option;
import javax.faces.FacesException;

/**
 * <p>Session scope data bean for your application.  Create properties
 *  here to represent cached data that should be made available across
 *  multiple HTTP requests for an individual user.</p>
 *
 * <p>An instance of this class will be created for you automatically,
 * the first time your application evaluates a value binding expression
 * or method binding expression that references a managed bean using
 * this class.</p>
 *
 * @version SessionBean1.java
 * @version Created on Nov 28, 2010, 5:30:40 AM
 * @author hcarralero
 */

public class SessionBean1 extends AbstractSessionBean {
    // <editor-fold defaultstate="collapsed" desc="Managed Component Definition">

    /**
     * <p>Automatically managed component initialization.  <strong>WARNING:</strong>
     * This method is automatically generated, so any user-specified code inserted
     * here is subject to being replaced.</p>
     */
    private void _init() throws Exception {
    }
    // </editor-fold>

    /**
     * <p>Construct a new session data bean instance.</p>
     */
    public SessionBean1() {
    }

    /**
     * <p>This method is called when this bean is initially added to
     * session scope.  Typically, this occurs as a result of evaluating
     * a value binding or method binding expression, which utilizes the
     * managed bean facility to instantiate this bean and store it into
     * session scope.</p>
     * 
     * <p>You may customize this method to initialize and cache data values
     * or resources that are required for the lifetime of a particular
     * user session.</p>
     */
    @Override
    public void init() {
        // Perform initializations inherited from our superclass
        super.init();
        // Perform application initialization that must complete
        // *before* managed components are initialized
        // TODO - add your own initialiation code here
        
        // <editor-fold defaultstate="collapsed" desc="Managed Component Initialization">
        // Initialize automatically managed components
        // *Note* - this logic should NOT be modified
        try {
            _init();
        } catch (Exception e) {
            log("SessionBean1 Initialization Failure", e);
            throw e instanceof FacesException ? (FacesException) e: new FacesException(e);
        }
        
        // </editor-fold>
        // Perform application initialization that must complete
        // *after* managed components are initialized
        // TODO - add your own initialization code here
    }

    /**
     * <p>This method is called when the session containing it is about to be
     * passivated.  Typically, this occurs in a distributed servlet container
     * when the session is about to be transferred to a different
     * container instance, after which the <code>activate()</code> method
     * will be called to indicate that the transfer is complete.</p>
     * 
     * <p>You may customize this method to release references to session data
     * or resources that can not be serialized with the session itself.</p>
     */
    @Override
    public void passivate() {
    }

    /**
     * <p>This method is called when the session containing it was
     * reactivated.</p>
     * 
     * <p>You may customize this method to reacquire references to session
     * data or resources that could not be serialized with the
     * session itself.</p>
     */
    @Override
    public void activate() {
    }

    /**
     * <p>This method is called when this bean is removed from
     * session scope.  Typically, this occurs as a result of
     * the session timing out or being terminated by the application.</p>
     * 
     * <p>You may customize this method to clean up resources allocated
     * during the execution of the <code>init()</code> method, or
     * at any later time during the lifetime of the application.</p>
     */
    @Override
    public void destroy() {
    }
    
    /**
     * <p>Return a reference to the scoped data bean.</p>
     *
     * @return reference to the scoped data bean
     */
    protected ApplicationBean1 getApplicationBean1() {
        return (ApplicationBean1) getBean("ApplicationBean1");
    }

    private TOLMachine tm = new TOLMachine();

    /**
     * Get the value of tm
     *
     * @return the value of tm
     */
    public TOLMachine getTm() {
        return tm;
    }

    /**
     * Set the value of tm
     *
     * @param tm new value of tm
     */
    public void setTm(TOLMachine tm) {
        this.tm = tm;
    }


    private TOLVariable[] vars = null;

    /**
     * Get the value of vars
     *
     * @return the value of vars
     */
    public TOLVariable[] getVars() {
        return vars;
    }

    /**
     * Set the value of vars
     *
     * @param vars new value of vars
     */
    public void setVars(TOLVariable[] vars) {
        this.vars = vars;
    }

    protected Option[] optVARS;

    /**
     * Get the value of optVARS
     *
     * @return the value of optVARS
     */
    public Option[] getOptVARS() {
        return optVARS;
    }

    private TOLVariable varActual;

    /**
     * Get the value of varActual
     *
     * @return the value of varActual
     */
    public TOLVariable getVarActual() {
        return varActual;
    }

    /**
     * Set the value of varActual
     *
     * @param varActual new value of varActual
     */
    public void setVarActual(TOLVariable varActual) {
        this.varActual = varActual;
    }

    private void actualizarOptVARS() {
        int nv= 0;
        if (vars!=null&&vars.length>0) {
            nv= vars.length;
            optVARS= new Option[nv];
            for (int i= 0; i<nv; ++i) {
                String name= vars[i].getName();
                if (name==null) {
                    name= "Unknow";
                } else {
                    name= name.trim();
                    if (name.length()==0) {
                        name= "Unknow";
                    }
                }
                String desc= vars[i].getDesc();
                if (desc!=null) {
                    desc= desc.trim();
                    if (desc.length()==0) {
                        desc= null;
                    }
                }
                String nd= name;
                if (desc!=null) {
                    nd+= "==>"+desc;
                }
                optVARS[i]= new Option(i, nd);
            }
        }
    }
    public void actualizarVARSText() {
        vars= tm.getTextVariables();
        actualizarOptVARS();
    }

    public void actualizarVARSReal() {
        vars= tm.getRealVariables();
        actualizarOptVARS();
    }

    public void actualizarVARSDate() {
        vars= tm.getDateVariables();
        actualizarOptVARS();
    }

    public void actualizarVARSComplex() {
        vars= tm.getComplexVariables();
        actualizarOptVARS();
    }

    public void actualizarVARSMatrix() {
        vars= tm.getMatrixVariables();
        actualizarOptVARS();
    }

    public void actualizarVARSVMatrix() {
        vars= tm.getVMatrixVariables();
        actualizarOptVARS();
    }

    public void actualizarVARSPolyn() {
        vars= tm.getPolynVariables();
        actualizarOptVARS();
    }

    public void actualizarVARSRatio() {
        vars= tm.getRatioVariables();
        actualizarOptVARS();
    }

    public void actualizarVARSCode() {
        vars= tm.getCodeVariables();
        actualizarOptVARS();
    }

    public void actualizarVARSSerie() {
        vars= tm.getSerieVariables();
        actualizarOptVARS();
    }

    public void actualizarVARSTimeSet() {
        vars= tm.getTimeSetVariables();
        actualizarOptVARS();
    }

    public void actualizarVARSSet() {
        vars= tm.getSetVariables();
        actualizarOptVARS();
    }

    private String varType= null;

    /**
     * Get the value of varType
     *
     * @return the value of varType
     */
    public String getVarType() {
        return varType;
    }

    /**
     * Set the value of varType
     *
     * @param varType new value of varType
     */
    public void setVarType(String varType) {
        this.varType = varType;
    }

}
