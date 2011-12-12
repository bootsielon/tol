/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package com.hcarralero.toljava.demo;

import com.hcarralero.toljava.TOLMachine;
import com.sun.rave.web.ui.appbase.AbstractPageBean;
import com.sun.webui.jsf.component.StaticText;
import com.sun.webui.jsf.component.TextArea;
import javax.faces.FacesException;

/**
 * <p>Page bean that corresponds to a similarly named JSP page.  This
 * class contains component definitions (and initialization code) for
 * all components that you have defined on this page, as well as
 * lifecycle methods and event handlers where you may add behavior
 * to respond to incoming events.</p>
 *
 * @version Docs.java
 * @version Created on Nov 28, 2010, 1:55:24 PM
 * @author hcarralero
 */

public class Interprete extends AbstractPageBean {
    // <editor-fold defaultstate="collapsed" desc="Managed Component Definition">

    /**
     * <p>Automatically managed component initialization.  <strong>WARNING:</strong>
     * This method is automatically generated, so any user-specified code inserted
     * here is subject to being replaced.</p>
     */
    private void _init() throws Exception {
    }
    private TextArea taCodigo = new TextArea();

    public TextArea getTaCodigo() {
        return taCodigo;
    }

    public void setTaCodigo(TextArea ta) {
        this.taCodigo = ta;
    }
    private StaticText stxtResultadoEjecutar = new StaticText();

    public StaticText getStxtResultadoEjecutar() {
        return stxtResultadoEjecutar;
    }

    public void setStxtResultadoEjecutar(StaticText st) {
        this.stxtResultadoEjecutar = st;
    }

    // </editor-fold>

    /**
     * <p>Construct a new Page bean instance.</p>
     */
    public Interprete() {
    }

    /**
     * <p>Callback method that is called whenever a page is navigated to,
     * either directly via a URL, or indirectly via page navigation.
     * Customize this method to acquire resources that will be needed
     * for event handlers and lifecycle methods, whether or not this
     * page is performing post back processing.</p>
     * 
     * <p>Note that, if the current request is a postback, the property
     * values of the components do <strong>not</strong> represent any
     * values submitted with this request.  Instead, they represent the
     * property values that were saved for this view when it was rendered.</p>
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
            log("Docs Initialization Failure", e);
            throw e instanceof FacesException ? (FacesException) e: new FacesException(e);
        }
        
        // </editor-fold>
        // Perform application initialization that must complete
        // *after* managed components are initialized
        // TODO - add your own initialization code here
        MenuIzquierdo mi= (MenuIzquierdo)this.getBean("MenuIzquierdo");
        if (mi!=null) {
            mi.getTrMenuIzquierdo().setSelected("tnInterprete");
        }
    }

    /**
     * <p>Callback method that is called after the component tree has been
     * restored, but before any event processing takes place.  This method
     * will <strong>only</strong> be called on a postback request that
     * is processing a form submit.  Customize this method to allocate
     * resources that will be required in your event handlers.</p>
     */
    @Override
    public void preprocess() {
    }

    /**
     * <p>Callback method that is called just before rendering takes place.
     * This method will <strong>only</strong> be called for the page that
     * will actually be rendered (and not, for example, on a page that
     * handled a postback and then navigated to a different page).  Customize
     * this method to allocate resources that will be required for rendering
     * this page.</p>
     */
    @Override
    public void prerender() {
    }

    /**
     * <p>Callback method that is called after rendering is completed for
     * this request, if <code>init()</code> was called (regardless of whether
     * or not this was the page that was actually rendered).  Customize this
     * method to release resources acquired in the <code>init()</code>,
     * <code>preprocess()</code>, or <code>prerender()</code> methods (or
     * acquired during execution of an event handler).</p>
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

    /**
     * <p>Return a reference to the scoped data bean.</p>
     *
     * @return reference to the scoped data bean
     */
    protected SessionBean1 getSessionBean1() {
        return (SessionBean1) getBean("SessionBean1");
    }

    /**
     * <p>Return a reference to the scoped data bean.</p>
     *
     * @return reference to the scoped data bean
     */
    protected RequestBean1 getRequestBean1() {
        return (RequestBean1) getBean("RequestBean1");
    }

    public String btnEjecutarCodigo_action() {
        TOLMachine tm= this.getSessionBean1().getTm();
        String code= (String)this.taCodigo.getText();
        Object res[]= tm.execute(code);
        String htmlRes="<html><h1 style=\"text-decoration:underline\">RESULTADOS DE LA EVALUACIÓN</h1>";
        if (res==null) {
            htmlRes+="<h2 style=\"color:red\">&nbsp;&nbsp;&nbsp;&nbsp;ERROR DE ACCESO A TOL</h2>";
        } else {
            try {
                int ne= Integer.parseInt((String)res[0]);
                htmlRes+="<h2 style=\"color:";
                if (ne>0) {
                    htmlRes+="red\">&nbsp;&nbsp;&nbsp;&nbsp;Se ha(n) detectado "+ne+" error(es)";
                } else {
                    htmlRes+="green\">&nbsp;&nbsp;&nbsp;&nbsp;No se han detectado errores";
                }
                htmlRes+= "</h2>";
                for (int i= 1; i<res.length; ++i) {
                    String strRes= (String)res[i];
                    int l= strRes.length();
                    htmlRes+="<h3";
                    if (strRes.charAt(0)=='<'&&strRes.charAt(2)=='>'&&(strRes.charAt(1)=='E'||strRes.charAt(1)=='W')&&l>=7) {
                        char tipo= strRes.charAt(1);
                        switch (tipo) {
                            case 'E':
                            case 'e':
                                htmlRes+=" style=\"color:red\"";
                                break;
                            case 'W':
                            case 'w':
                                htmlRes+=" style=\"color:yellow\"";
                                break;
                            default:
                                htmlRes+=" style=\"color:green\"";
                        }
                        htmlRes+= ">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;"+strRes.substring(3, l-5);
                    } else {
                        htmlRes+= ">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;"+strRes;
                    }
                    htmlRes+="</h3>";
                }
            } catch (NumberFormatException ex) {
                htmlRes+="<h2 style=\"color:red\">&nbsp;&nbsp;&nbsp;&nbsp;ERROR DE ACCESO AL NÚMERO DE ERRORES</h2>";
            }
        }
        htmlRes+="</html>";
        this.stxtResultadoEjecutar.setText(htmlRes);
        return null;
    }
    
}

