/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package com.hcarralero.toljava.demo;

import com.hcarralero.toljava.TOLCode;
import com.hcarralero.toljava.TOLComplex;
import com.hcarralero.toljava.TOLDate;
import com.hcarralero.toljava.TOLDateException;
import com.hcarralero.toljava.TOLMachine;
import com.hcarralero.toljava.TOLMachineException;
import com.hcarralero.toljava.TOLMatrix;
import com.hcarralero.toljava.TOLMatrixException;
import com.hcarralero.toljava.TOLObject;
import com.hcarralero.toljava.TOLPolyn;
import com.hcarralero.toljava.TOLRSerie;
import com.hcarralero.toljava.TOLRSet;
import com.hcarralero.toljava.TOLRTimeSet;
import com.hcarralero.toljava.TOLRatio;
import com.hcarralero.toljava.TOLReal;
import com.hcarralero.toljava.TOLText;
import com.hcarralero.toljava.TOLVMatrix;
import com.hcarralero.toljava.TOLVariable;
import com.sun.data.provider.RowKey;
import com.sun.rave.web.ui.appbase.AbstractPageBean;
import com.sun.webui.jsf.component.Button;
import com.sun.webui.jsf.component.Calendar;
import com.sun.webui.jsf.component.PanelLayout;
import com.sun.webui.jsf.component.RadioButtonGroup;
import com.sun.webui.jsf.component.Table;
import com.sun.webui.jsf.component.TextArea;
import com.sun.webui.jsf.component.TextField;
import com.sun.webui.jsf.model.SingleSelectOptionsList;
import java.util.Date;
import java.util.Iterator;
import java.util.List;
import javax.faces.FacesException;
import javax.faces.component.UIComponent;

/**
 * <p>Page bean that corresponds to a similarly named JSP page.  This
 * class contains component definitions (and initialization code) for
 * all components that you have defined on this page, as well as
 * lifecycle methods and event handlers where you may add behavior
 * to respond to incoming events.</p>
 *
 * @version Demo.java
 * @version Created on Nov 28, 2010, 5:30:41 AM
 * @author hcarralero
 */
public class Explorador extends AbstractPageBean {
    // <editor-fold defaultstate="collapsed" desc="Managed Component Definition">

    /**
     * <p>Automatically managed component initialization.  <strong>WARNING:</strong>
     * This method is automatically generated, so any user-specified code inserted
     * here is subject to being replaced.</p>
     */
    private void _init() throws Exception {
        rbgTipoVariableDefaultOptions.setOptions(new com.sun.webui.jsf.model.Option[]{new com.sun.webui.jsf.model.Option("T", "Text"), new com.sun.webui.jsf.model.Option("R", "Real"), new com.sun.webui.jsf.model.Option("D", "Date"), new com.sun.webui.jsf.model.Option("C", "Complex"), new com.sun.webui.jsf.model.Option("M", "Matrix"), new com.sun.webui.jsf.model.Option("V", "VMatrix"),new com.sun.webui.jsf.model.Option("P", "Polyn"),new com.sun.webui.jsf.model.Option("A", "Ratio"),new com.sun.webui.jsf.model.Option("F", "Code"),new com.sun.webui.jsf.model.Option("I", "TimeSet"),new com.sun.webui.jsf.model.Option("S", "Serie"), new com.sun.webui.jsf.model.Option("E", "Set")});
        rbgTipoVariableDefaultOptions.setSelectedValue("T");
    }
    private SingleSelectOptionsList rbgTipoVariableDefaultOptions = new SingleSelectOptionsList();

    public SingleSelectOptionsList getRbgTipoVariableDefaultOptions() {
        return rbgTipoVariableDefaultOptions;
    }

    public void setRbgTipoVariableDefaultOptions(SingleSelectOptionsList ssol) {
        this.rbgTipoVariableDefaultOptions = ssol;
    }
    private PanelLayout pnlDatosVAR = new PanelLayout();

    public PanelLayout getPnlDatosVAR() {
        return pnlDatosVAR;
    }

    public void setPnlDatosVAR(PanelLayout pl) {
        this.pnlDatosVAR = pl;
    }
    private RadioButtonGroup rbgTipoVariable = new RadioButtonGroup();

    public RadioButtonGroup getRbgTipoVariable() {
        return rbgTipoVariable;
    }

    public void setRbgTipoVariable(RadioButtonGroup rbg) {
        this.rbgTipoVariable = rbg;
    }
    private Table tblVARS = new Table();

    public Table getTblVARS() {
        return tblVARS;
    }

    public void setTblVARS(Table t) {
        this.tblVARS = t;
    }
    private TextField txtVARName = new TextField();

    public TextField getTxtVARName() {
        return txtVARName;
    }

    public void setTxtVARName(TextField tf) {
        this.txtVARName = tf;
    }
    private TextArea taVARDesc = new TextArea();

    public TextArea getTaVARDesc() {
        return taVARDesc;
    }

    public void setTaVARDesc(TextArea ta) {
        this.taVARDesc = ta;
    }
    private PanelLayout pnlEditarVARReal = new PanelLayout();

    public PanelLayout getPnlEditarVARReal() {
        return pnlEditarVARReal;
    }

    public void setPnlEditarVARReal(PanelLayout pl) {
        this.pnlEditarVARReal = pl;
    }
    private PanelLayout pnlEditarVARText = new PanelLayout();

    public PanelLayout getPnlEditarVARText() {
        return pnlEditarVARText;
    }

    public void setPnlEditarVARText(PanelLayout pl) {
        this.pnlEditarVARText = pl;
    }
    private TextArea taVARTextText = new TextArea();

    public TextArea getTaVARTextText() {
        return taVARTextText;
    }

    public void setTaVARTextText(TextArea ta) {
        this.taVARTextText = ta;
    }
    private TextField txtVARRealValue = new TextField();

    public TextField getTxtVARRealValue() {
        return txtVARRealValue;
    }

    public void setTxtVARRealValue(TextField tf) {
        this.txtVARRealValue = tf;
    }
    private PanelLayout pnlEditarVARDate = new PanelLayout();

    public PanelLayout getPnlEditarVARDate() {
        return pnlEditarVARDate;
    }

    public void setPnlEditarVARDate(PanelLayout pl) {
        this.pnlEditarVARDate = pl;
    }
    private Calendar calVARDateDate = new Calendar();

    public Calendar getCalVARDateDate() {
        return calVARDateDate;
    }

    public void setCalVARDateDate(Calendar c) {
        this.calVARDateDate = c;
    }
    private PanelLayout pnlEditarVARComplex = new PanelLayout();

    public PanelLayout getPnlEditarVARComplex() {
        return pnlEditarVARComplex;
    }

    public void setPnlEditarVARComplex(PanelLayout pl) {
        this.pnlEditarVARComplex = pl;
    }
    private TextField txtVARComplexX = new TextField();

    public TextField getTxtVARComplexX() {
        return txtVARComplexX;
    }

    public void setTxtVARComplexX(TextField tf) {
        this.txtVARComplexX = tf;
    }
    private TextField txtVARComplexY = new TextField();

    public TextField getTxtVARComplexY() {
        return txtVARComplexY;
    }

    public void setTxtVARComplexY(TextField tf) {
        this.txtVARComplexY = tf;
    }
    private Button btnVerVARS = new Button();

    public Button getBtnVerVARS() {
        return btnVerVARS;
    }

    public void setBtnVerVARS(Button b) {
        this.btnVerVARS = b;
    }
    private PanelLayout pnlEditarVARMatrix = new PanelLayout();

    public PanelLayout getPnlEditarVARMatrix() {
        return pnlEditarVARMatrix;
    }

    public void setPnlEditarVARMatrix(PanelLayout pl) {
        this.pnlEditarVARMatrix = pl;
    }
    private PanelLayout pnlAccionesVARMatrix = new PanelLayout();

    public PanelLayout getPnlAccionesVARMatrix() {
        return pnlAccionesVARMatrix;
    }

    public void setPnlAccionesVARMatrix(PanelLayout pl) {
        this.pnlAccionesVARMatrix = pl;
    }
    private PanelLayout pnlEditarVARVMatrix = new PanelLayout();

    public PanelLayout getPnlEditarVARVMatrix() {
        return pnlEditarVARVMatrix;
    }

    public void setPnlEditarVARVMatrix(PanelLayout pl) {
        this.pnlEditarVARVMatrix = pl;
    }
    private PanelLayout pnlAccionesVARVMatrix = new PanelLayout();

    public PanelLayout getPnlAccionesVARVMatrix() {
        return pnlAccionesVARVMatrix;
    }

    public void setPnlAccionesVARVMatrix(PanelLayout pl) {
        this.pnlAccionesVARVMatrix = pl;
    }
    private PanelLayout pnlEditarVARPolyn = new PanelLayout();

    public PanelLayout getPnlEditarVARPolyn() {
        return pnlEditarVARPolyn;
    }

    public void setPnlEditarVARPolyn(PanelLayout pl) {
        this.pnlEditarVARPolyn = pl;
    }
    private PanelLayout layoutPanel5 = new PanelLayout();

    public PanelLayout getLayoutPanel5() {
        return layoutPanel5;
    }

    public void setLayoutPanel5(PanelLayout pl) {
        this.layoutPanel5 = pl;
    }
    private PanelLayout pnlEditarVARRatio = new PanelLayout();

    public PanelLayout getPnlEditarVARRatio() {
        return pnlEditarVARRatio;
    }

    public void setPnlEditarVARRatio(PanelLayout pl) {
        this.pnlEditarVARRatio = pl;
    }
    private TextField txtVARPolyn = new TextField();

    public TextField getTxtVARPolyn() {
        return txtVARPolyn;
    }

    public void setTxtVARPolyn(TextField tf) {
        this.txtVARPolyn = tf;
    }
    private TextField txtVARRatio = new TextField();

    public TextField getTxtVARRatio() {
        return txtVARRatio;
    }

    public void setTxtVARRatio(TextField tf) {
        this.txtVARRatio = tf;
    }
    private TextField txtVARCodeInfo = new TextField();

    public TextField getTxtVARCodeInfo() {
        return txtVARCodeInfo;
    }

    public void setTxtVARCodeInfo(TextField tf) {
        this.txtVARCodeInfo = tf;
    }
    private PanelLayout pnlEditarVARCode = new PanelLayout();

    public PanelLayout getPnlEditarVARCode() {
        return pnlEditarVARCode;
    }

    public void setPnlEditarVARCode(PanelLayout pl) {
        this.pnlEditarVARCode = pl;
    }
    private TextField txtVARCodeName = new TextField();

    public TextField getTxtVARCodeName() {
        return txtVARCodeName;
    }

    public void setTxtVARCodeName(TextField tf) {
        this.txtVARCodeName = tf;
    }
    private PanelLayout pnlEditarVARSerie = new PanelLayout();

    public PanelLayout getPnlEditarVARSerie() {
        return pnlEditarVARSerie;
    }

    public void setPnlEditarVARSerie(PanelLayout pl) {
        this.pnlEditarVARSerie = pl;
    }
    private TextField txtVARSerie = new TextField();

    public TextField getTxtVARSerie() {
        return txtVARSerie;
    }

    public void setTxtVARSerie(TextField tf) {
        this.txtVARSerie = tf;
    }
    private PanelLayout pnlEditarVARTimeSet = new PanelLayout();

    public PanelLayout getPnlEditarVARTimeSet() {
        return pnlEditarVARTimeSet;
    }

    public void setPnlEditarVARTimeSet(PanelLayout pl) {
        this.pnlEditarVARTimeSet = pl;
    }
    private TextField txtVARTimeSet = new TextField();

    public TextField getTxtVARTimeSet() {
        return txtVARTimeSet;
    }

    public void setTxtVARTimeSet(TextField tf) {
        this.txtVARTimeSet = tf;
    }
    private PanelLayout pnlEditarVARSet = new PanelLayout();

    public PanelLayout getPnlEditarVARSet() {
        return pnlEditarVARSet;
    }

    public void setPnlEditarVARSet(PanelLayout pl) {
        this.pnlEditarVARSet = pl;
    }
    private TextField txtVARSet = new TextField();

    public TextField getTxtVARSet() {
        return txtVARSet;
    }

    public void setTxtVARSet(TextField tf) {
        this.txtVARSet = tf;
    }

    // </editor-fold>
    /**
     * <p>Construct a new Page bean instance.</p>
     */
    public Explorador() {
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
            log("Page1 Initialization Failure", e);
            throw e instanceof FacesException ? (FacesException) e : new FacesException(e);
        }

        // </editor-fold>
        // Perform application initialization that must complete
        // *after* managed components are initialized
        // TODO - add your own initialization code here
        MenuIzquierdo mi = (MenuIzquierdo) this.getBean("MenuIzquierdo");
        if (mi != null) {
            mi.getTrMenuIzquierdo().setSelected("tnExplorador");
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

    /**
     * <p>Return a reference to the scoped data bean.</p>
     *
     * @return reference to the scoped data bean
     */
    protected ApplicationBean1 getApplicationBean1() {
        return (ApplicationBean1) getBean("ApplicationBean1");
    }

    public String getVARValor() {
        RowKey rkPAI = (RowKey) getValue("#{currentRow.tableRow}");
        int varN = Integer.parseInt(rkPAI.getRowId());
        TOLVariable var = this.getSessionBean1().getVars()[varN];
        TOLObject value= var.getValue();
        if (value==null) {
            return "<<null>>";
        }
        return value.toString();
    }

    public String btnVerVARS_action() {
        String tipo = "<DESCONOCIDO>";
        String strtv = ((String) this.rbgTipoVariable.getSelected());
        if (strtv == null) {
            error("Debe seleccionar el tipo de variable");
            this.getSessionBean1().setVars(null);
        } else {
            char tv = strtv.charAt(0);
            switch (tv) {
                case 'T':
                    this.getSessionBean1().actualizarVARSText();
                    tipo = "<Text>";
                    break;
                case 'R':
                    this.getSessionBean1().actualizarVARSReal();
                    tipo = "<Real>";
                    break;
                case 'D':
                    this.getSessionBean1().actualizarVARSDate();
                    tipo = "<Date>";
                    break;
                case 'C':
                    this.getSessionBean1().actualizarVARSComplex();
                    tipo = "<Complex>";
                    break;
                case 'M':
                    this.getSessionBean1().actualizarVARSMatrix();
                    tipo = "<Matrix>";
                    break;
                case 'V':
                    this.getSessionBean1().actualizarVARSVMatrix();
                    tipo = "<VMatrix>";
                    break;
                case 'P':
                    this.getSessionBean1().actualizarVARSPolyn();
                    tipo = "<Polyn>";
                    break;
                case 'A':
                    this.getSessionBean1().actualizarVARSRatio();
                    tipo = "<Ratio>";
                    break;
                case 'F':
                    this.getSessionBean1().actualizarVARSCode();
                    tipo = "<Code>";
                    break;
                case 'I':
                    this.getSessionBean1().actualizarVARSTimeSet();
                    tipo = "<TimeSet>";
                    break;
                case 'S':
                    this.getSessionBean1().actualizarVARSSerie();
                    tipo = "<Serie>";
                    break;
                case 'E':
                    this.getSessionBean1().actualizarVARSSet();
                    tipo = "<Set>";
                    break;
            }
            this.tblVARS.setTitle("Variables de tipo " + tipo + " que se han definido");
            this.tblVARS.setVisible(true);
        }
        return null;
    }

    public String btnEditarVAR_action() {
        RowKey rkPAI = (RowKey) getValue("#{currentRow.tableRow}");
        int varN = Integer.parseInt(rkPAI.getRowId());
        TOLVariable var = this.getSessionBean1().getVars()[varN];
        this.getSessionBean1().setVarActual(var);
        this.txtVARName.setText(var.getName());
        this.taVARDesc.setText(var.getDesc());
        this.pnlDatosVAR.setVisible(true);
        TOLObject to = var.getValue();
        if (to.getClass() == TOLText.class) {
            TOLText tt = (TOLText) to;
            this.taVARTextText.setText(tt.getText());
            this.pnlEditarVARText.setVisible(true);
        } else {
            if (to.getClass() == TOLReal.class) {
                TOLReal tr = (TOLReal) to;
                this.txtVARRealValue.setText("" + tr.getValue());
                this.pnlEditarVARReal.setVisible(true);
            } else {
                if (to.getClass() == TOLDate.class) {
                    TOLDate td = (TOLDate) to;
                    java.util.Calendar cal = java.util.Calendar.getInstance();
                    cal.set(td.getYear(), td.getMonth(), td.getDay(), td.getHour(), td.getMinute(), (int) td.getSecond());
                    Date date = cal.getTime();
                    this.calVARDateDate.setSelectedDate(date);
                    this.pnlEditarVARDate.setVisible(true);
                } else {
                    if (to.getClass() == TOLComplex.class) {
                        TOLComplex tc = (TOLComplex) to;
                        this.txtVARComplexX.setText("" + tc.getX());
                        this.txtVARComplexY.setText("" + tc.getY());
                        this.pnlEditarVARComplex.setVisible(true);
                    } else {
                        if (to.getClass() == TOLMatrix.class) {
                            TOLMatrix tm = (TOLMatrix) to;
                            int rows = tm.getRows();
                            int columns = tm.getColumns();
                            int height = 94 + rows * 32;
                            String style = "border-top: solid navy 1px; border-bottom: solid navy 1px; width: 100%; -rave-layout: grid;  height: " + height + "px;";
                            this.pnlEditarVARMatrix.setStyle(style);
                            int topAcciones = 48 + 32 * rows;
                            String styleAcciones = "border-width: 1px; border-style: dashed; background-color: rgb(130, 154, 179); height: 32px; left: 20%; position: absolute; text-align: center; width: 60%; top: " + topAcciones + "px";
                            this.pnlAccionesVARMatrix.setStyle(styleAcciones);
                            List<UIComponent> lcmat = this.pnlEditarVARMatrix.getChildren();
                            for (int i = 0; i < rows; ++i) {
                                int topFila = 48 + 32 * i;
                                String styleFila = "height: 32px; left: 5%; top: " + topFila + "px; position: absolute; text-align: center; width: 90%";
                                com.sun.webui.jsf.component.PanelLayout pnlFila = new com.sun.webui.jsf.component.PanelLayout();
                                pnlFila.setId("fila" + i);
                                pnlFila.setStyle(styleFila);
                                lcmat.add(pnlFila);
                                pnlFila.setParent(this.pnlEditarVARMatrix);
                                List<UIComponent> lcfila = pnlFila.getChildren();
                                for (int j = 0; j < columns; ++j) {
                                    String styleCelda = "height: 24px; margin-top: 4px";
                                    com.sun.webui.jsf.component.TextField txtCelda = new com.sun.webui.jsf.component.TextField();
                                    txtCelda.setId("celda" + i + "" + j);
                                    txtCelda.setColumns(8);
                                    txtCelda.setStyle(styleCelda);
                                    try {
                                        txtCelda.setText("" + tm.getCellValue(i, j));
                                    } catch (TOLMatrixException ex) {
                                        txtCelda.setText("<ERROR>");
                                        txtCelda.setDisabled(true);
                                        error("ERROR accediendo a la matriz: " + ex.getMessage());
                                    }
                                    lcfila.add(txtCelda);
                                    txtCelda.setParent(pnlFila);
                                }
                            }
                            this.pnlEditarVARMatrix.setVisible(true);
                        } else {
                            if (to.getClass() == TOLVMatrix.class) {
                                TOLVMatrix tvm = (TOLVMatrix) to;
                                int rows = tvm.getRows();
                                int columns = tvm.getColumns();
                                int height = 94 + rows * 32;
                                String style = "border-top: solid navy 1px; border-bottom: solid navy 1px; width: 100%; -rave-layout: grid;  height: " + height + "px;";
                                this.pnlEditarVARVMatrix.setStyle(style);
                                int topAcciones = 48 + 32 * rows;
                                String styleAcciones = "border-width: 1px; border-style: dashed; background-color: rgb(130, 154, 179); height: 32px; left: 20%; position: absolute; text-align: center; width: 60%; top: " + topAcciones + "px";
                                this.pnlAccionesVARVMatrix.setStyle(styleAcciones);
                                List<UIComponent> lcvmat = this.pnlEditarVARVMatrix.getChildren();
                                for (int i = 0; i < rows; ++i) {
                                    int topFila = 48 + 32 * i;
                                    String styleFila = "height: 32px; left: 5%; top: " + topFila + "px; position: absolute; text-align: center; width: 90%";
                                    com.sun.webui.jsf.component.PanelLayout pnlFila = new com.sun.webui.jsf.component.PanelLayout();
                                    pnlFila.setId("fila" + i);
                                    pnlFila.setStyle(styleFila);
                                    lcvmat.add(pnlFila);
                                    pnlFila.setParent(this.pnlEditarVARVMatrix);
                                    List<UIComponent> lcfila = pnlFila.getChildren();
                                    for (int j = 0; j < columns; ++j) {
                                        String styleCelda = "height: 24px; margin-top: 4px";
                                        com.sun.webui.jsf.component.TextField txtCelda = new com.sun.webui.jsf.component.TextField();
                                        txtCelda.setId("celda" + i + "" + j);
                                        txtCelda.setColumns(8);
                                        txtCelda.setStyle(styleCelda);
                                        try {
                                            txtCelda.setText("" + tvm.getCellValue(i, j));
                                        } catch (TOLMatrixException ex) {
                                            txtCelda.setText("<ERROR>");
                                            txtCelda.setDisabled(true);
                                            error("ERROR accediendo a la matriz: " + ex.getMessage());
                                        }
                                        lcfila.add(txtCelda);
                                        txtCelda.setParent(pnlFila);
                                    }
                                }
                                this.pnlEditarVARVMatrix.setVisible(true);
                            } else {
                                if (to.getClass() == TOLPolyn.class) {
                                    TOLPolyn tpol= (TOLPolyn)to;
                                    this.txtVARPolyn.setText(tpol.toString());
                                    this.pnlEditarVARPolyn.setVisible(true);
                                } else {
                                    if (to.getClass() == TOLRatio.class) {
                                        TOLRatio trat= (TOLRatio)to;
                                        this.txtVARRatio.setText(trat.toString());
                                        this.pnlEditarVARRatio.setVisible(true);
                                    } else {
                                        if (to.getClass() == TOLCode.class) {
                                            TOLCode tcod= (TOLCode)to;
                                            this.txtVARCodeName.setText(tcod.getFunction());
                                            this.txtVARCodeInfo.setText(tcod.getInfo());
                                            this.pnlEditarVARCode.setVisible(true);
                                        } else {
                                            if (to.getClass() == TOLRTimeSet.class) {
                                                TOLRTimeSet ts= (TOLRTimeSet)to;
                                                this.txtVARTimeSet.setText(ts.toString());
                                                this.pnlEditarVARTimeSet.setVisible(true);
                                            } else {
                                                if (to.getClass() == TOLRSerie.class) {
                                                    TOLRSerie ts= (TOLRSerie)to;
                                                    this.txtVARSerie.setText(ts.toString());
                                                    this.pnlEditarVARSerie.setVisible(true);
                                                } else {
                                                    if (to.getClass() == TOLRSet.class) {
                                                        TOLRSet ts= (TOLRSet)to;
                                                        this.txtVARSet.setText(ts.toString());
                                                        this.pnlEditarVARSet.setVisible(true);
                                                    } else {
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        this.rbgTipoVariable.setDisabled(true);
        this.btnVerVARS.setDisabled(true);
        this.tblVARS.setVisible(false);
        return null;
    }

    public String btnModificarVARText_action() {
        int lText = 0;
        String text = (String) this.taVARTextText.getText();
        if (text != null) {
            lText = text.length();
        }
        if (lText == 0) {
            error("ERROR: Debe escribir el nuevo texto para la variable.");
        } else {
            try {
                TOLText tt = new TOLText(text);
                TOLVariable var = this.getSessionBean1().getVarActual();
                this.getSessionBean1().getTm().setVariableValue(var.getName(), tt);
                this.pnlDatosVAR.setVisible(false);
                this.pnlEditarVARText.setVisible(false);
                this.rbgTipoVariable.setDisabled(false);
                this.rbgTipoVariable.setSelected("T");
                this.btnVerVARS.setDisabled(false);
                this.getSessionBean1().actualizarVARSText();
                this.tblVARS.setVisible(true);
            } catch (TOLMachineException ex) {
                error("ERROR: " + ex.getMessage());
            }
        }
        return null;
    }

    public String btnCancelarVARText_action() {
        this.pnlDatosVAR.setVisible(false);
        this.pnlEditarVARText.setVisible(false);
        this.rbgTipoVariable.setDisabled(false);
        this.rbgTipoVariable.setSelected("T");
        this.btnVerVARS.setDisabled(false);
        this.getSessionBean1().actualizarVARSText();
        this.tblVARS.setVisible(true);
        return null;
    }

    public String btnModificarVARReal_action() {
        int lText = 0;
        String text = (String) this.txtVARRealValue.getText();
        if (text != null) {
            lText = text.length();
        }
        if (lText == 0) {
            error("ERROR: Debe escribir el nuevo valor para la variable.");
        } else {
            try {
                double value = Double.parseDouble(text);
                TOLReal tr = new TOLReal(value);
                TOLVariable var = this.getSessionBean1().getVarActual();
                this.getSessionBean1().getTm().setVariableValue(var.getName(), tr);
                this.pnlDatosVAR.setVisible(false);
                this.pnlEditarVARReal.setVisible(false);
                this.rbgTipoVariable.setDisabled(false);
                this.rbgTipoVariable.setSelected("R");
                this.btnVerVARS.setDisabled(false);
                this.getSessionBean1().actualizarVARSReal();
                this.tblVARS.setVisible(true);
            } catch (NumberFormatException ex) {
                error("ERROR: El valor ingresado no es un número.");
            } catch (TOLMachineException ex) {
                error("ERROR: " + ex.getMessage());
            }
        }
        return null;
    }

    public String btnCancelarVARReal_action() {
        this.pnlDatosVAR.setVisible(false);
        this.pnlEditarVARReal.setVisible(false);
        this.rbgTipoVariable.setDisabled(false);
        this.rbgTipoVariable.setSelected("R");
        this.btnVerVARS.setDisabled(false);
        this.getSessionBean1().actualizarVARSReal();
        this.tblVARS.setVisible(true);
        return null;
    }

    public String btnModificarVARDate_action() {
        Date date = this.calVARDateDate.getSelectedDate();
        if (date == null) {
            error("ERROR: Debe seleccionar la nueva fecha.");
        } else {
            try {
                java.util.Calendar cal = java.util.Calendar.getInstance();
                cal.setTime(date);
                TOLDate td = new TOLDate(cal.get(java.util.Calendar.YEAR), cal.get(java.util.Calendar.MONTH), cal.get(java.util.Calendar.DAY_OF_MONTH), cal.get(java.util.Calendar.HOUR), cal.get(java.util.Calendar.MINUTE), cal.get(java.util.Calendar.SECOND));
                TOLVariable var = this.getSessionBean1().getVarActual();
                this.getSessionBean1().getTm().setVariableValue(var.getName(), td);
                this.pnlDatosVAR.setVisible(false);
                this.pnlEditarVARDate.setVisible(false);
                this.rbgTipoVariable.setDisabled(false);
                this.rbgTipoVariable.setSelected("D");
                this.btnVerVARS.setDisabled(false);
                this.getSessionBean1().actualizarVARSDate();
                this.tblVARS.setVisible(true);
            } catch (TOLDateException ex) {
                error("ERROR en fecha: " + ex.getMessage());
            } catch (TOLMachineException ex) {
                error("ERROR en TOLMachine: " + ex.getMessage());
            }
        }
        return null;
    }

    public String btnCancelarVARDate_action() {
        this.pnlDatosVAR.setVisible(false);
        this.pnlEditarVARDate.setVisible(false);
        this.rbgTipoVariable.setDisabled(false);
        this.rbgTipoVariable.setSelected("D");
        this.btnVerVARS.setDisabled(false);
        this.getSessionBean1().actualizarVARSDate();
        this.tblVARS.setVisible(true);
        return null;
    }

    public String btnModificarVARComplex_action() {
        int lXText = 0;
        String xText = (String) this.txtVARComplexX.getText();
        if (xText != null) {
            lXText = xText.length();
        }
        if (lXText == 0) {
            error("ERROR: Debe escribir el nuevo valor para X.");
        } else {
            int lYText = 0;
            String yText = (String) this.txtVARComplexY.getText();
            if (yText != null) {
                lYText = yText.length();
            }
            if (lYText == 0) {
                error("ERROR: Debe escribir el nuevo valor para Y.");
            } else {
                try {
                    double x = Double.parseDouble(xText);
                    double y = Double.parseDouble(yText);
                    TOLComplex tc = new TOLComplex(x, y);
                    TOLVariable var = this.getSessionBean1().getVarActual();
                    this.getSessionBean1().getTm().setVariableValue(var.getName(), tc);
                    this.pnlDatosVAR.setVisible(false);
                    this.pnlEditarVARComplex.setVisible(false);
                    this.rbgTipoVariable.setDisabled(false);
                    this.rbgTipoVariable.setSelected("C");
                    this.btnVerVARS.setDisabled(false);
                    this.getSessionBean1().actualizarVARSComplex();
                    this.tblVARS.setVisible(true);
                } catch (NumberFormatException ex) {
                    error("ERROR: Al menos uno de los valores ingresados no es un número.");
                } catch (TOLMachineException ex) {
                    error("ERROR: " + ex.getMessage());
                }
            }
        }
        return null;
    }

    public String btnCancelarVARComplex_action() {
        this.pnlDatosVAR.setVisible(false);
        this.pnlEditarVARComplex.setVisible(false);
        this.rbgTipoVariable.setDisabled(false);
        this.rbgTipoVariable.setSelected("C");
        this.btnVerVARS.setDisabled(false);
        this.getSessionBean1().actualizarVARSComplex();
        this.tblVARS.setVisible(true);
        return null;
    }

    private void eliminarPanelesFilasMatrix() {
        List<UIComponent> lcmat = this.pnlEditarVARMatrix.getChildren();
        Iterator<UIComponent> itmat = lcmat.iterator();
        while (itmat.hasNext()) {
            UIComponent comp = itmat.next();
            if (comp.getId().contains("fila")) {
                itmat.remove();
            }
        }
    }

    public String btnModificarVARMatrix_action() {
        Iterator<UIComponent> itmat = this.pnlEditarVARMatrix.getChildren().iterator();
        UIComponent comp = null;
        while (itmat.hasNext()) {
            comp = itmat.next();
            if (comp.getId().equals("fila0")) {
                break;
            }
        }
        TOLVariable var = this.getSessionBean1().getVarActual();
        TOLMatrix mat = (TOLMatrix) var.getValue();
        try {
            for (int i = 0; i < mat.getRows(); ++i) {
                com.sun.webui.jsf.component.PanelLayout pnlFila = (com.sun.webui.jsf.component.PanelLayout) comp;
                Iterator<UIComponent> itfil = pnlFila.getChildren().iterator();
                for (int j = 0; j < mat.getColumns(); ++j) {
                    com.sun.webui.jsf.component.TextField txtCelda = (com.sun.webui.jsf.component.TextField) itfil.next();
                    double valueij = Double.parseDouble((String) txtCelda.getText());
                    mat.setCellValue(i, j, valueij);
                }
                if (itmat.hasNext()) {
                    comp = itmat.next();
                }
            }
            this.getSessionBean1().getTm().setVariableValue(var.getName(), mat);
            this.eliminarPanelesFilasMatrix();
            this.pnlDatosVAR.setVisible(false);
            this.pnlEditarVARMatrix.setVisible(false);
            this.rbgTipoVariable.setDisabled(false);
            this.rbgTipoVariable.setSelected("M");
            this.btnVerVARS.setDisabled(false);
            this.getSessionBean1().actualizarVARSMatrix();
            this.tblVARS.setVisible(true);
        } catch (NumberFormatException ex) {
            error("ERROR: No se ha ingresado un número en alguna celda ");
        } catch (TOLMatrixException ex) {
            error("ERROR accediendo a la matriz: " + ex.getMessage());
        } catch (TOLMachineException ex) {
            error("ERROR: " + ex.getMessage());
        }
        return null;
    }

    public String btnCancelarVARMatrix_action() {
        this.eliminarPanelesFilasMatrix();
        this.pnlDatosVAR.setVisible(false);
        this.pnlEditarVARMatrix.setVisible(false);
        this.rbgTipoVariable.setDisabled(false);
        this.rbgTipoVariable.setSelected("M");
        this.btnVerVARS.setDisabled(false);
        this.getSessionBean1().actualizarVARSMatrix();
        this.tblVARS.setVisible(true);
        return null;
    }

    private void eliminarPanelesFilasVMatrix() {
        List<UIComponent> lcmat = this.pnlEditarVARVMatrix.getChildren();
        Iterator<UIComponent> itmat = lcmat.iterator();
        while (itmat.hasNext()) {
            UIComponent comp = itmat.next();
            if (comp.getId().contains("fila")) {
                itmat.remove();
            }
        }
    }

    public String btnModificarVARVMatrix_action() {
        Iterator<UIComponent> itvmat = this.pnlEditarVARVMatrix.getChildren().iterator();
        UIComponent comp = null;
        while (itvmat.hasNext()) {
            comp = itvmat.next();
            if (comp.getId().equals("fila0")) {
                break;
            }
        }
        TOLVariable var = this.getSessionBean1().getVarActual();
        TOLVMatrix vmat = (TOLVMatrix) var.getValue();
        try {
            for (int i = 0; i < vmat.getRows(); ++i) {
                com.sun.webui.jsf.component.PanelLayout pnlFila = (com.sun.webui.jsf.component.PanelLayout) comp;
                Iterator<UIComponent> itfil = pnlFila.getChildren().iterator();
                for (int j = 0; j < vmat.getColumns(); ++j) {
                    com.sun.webui.jsf.component.TextField txtCelda = (com.sun.webui.jsf.component.TextField) itfil.next();
                    double valueij = Double.parseDouble((String) txtCelda.getText());
                    vmat.setCellValue(i, j, valueij);
                }
                if (itvmat.hasNext()) {
                    comp = itvmat.next();
                }
            }
            this.getSessionBean1().getTm().setVariableValue(var.getName(), vmat);
            this.eliminarPanelesFilasVMatrix();
            this.pnlDatosVAR.setVisible(false);
            this.pnlEditarVARVMatrix.setVisible(false);
            this.rbgTipoVariable.setDisabled(false);
            this.rbgTipoVariable.setSelected("V");
            this.btnVerVARS.setDisabled(false);
            this.getSessionBean1().actualizarVARSVMatrix();
            this.tblVARS.setVisible(true);
        } catch (NumberFormatException ex) {
            error("ERROR: No se ha ingresado un número en alguna celda ");
        } catch (TOLMatrixException ex) {
            error("ERROR accediendo a la matriz: " + ex.getMessage());
        } catch (TOLMachineException ex) {
            error("ERROR: " + ex.getMessage());
        }
        return null;
    }

    public String btnCancelarVARVMatrix_action() {
        this.eliminarPanelesFilasVMatrix();
        this.pnlDatosVAR.setVisible(false);
        this.pnlEditarVARVMatrix.setVisible(false);
        this.rbgTipoVariable.setDisabled(false);
        this.rbgTipoVariable.setSelected("V");
        this.btnVerVARS.setDisabled(false);
        this.getSessionBean1().actualizarVARSVMatrix();
        this.tblVARS.setVisible(true);
        return null;
    }

    public String btnModificarVARPolyn_action() {
        TOLMachine tm= this.getSessionBean1().getTm();
        TOLVariable var= this.getSessionBean1().getVarActual();
        if (var==null) {
            error("ERROR: No se puede acceder a la Variable.");
        } else {
            String code= var.getName()+":="+(String)this.txtVARPolyn.getText();
            Object res[]= tm.execute(code);
            if (res==null) {
                error("ERROR: No se pudo acceder al Intérprete de TOL.");
            } else {
                try {
                    int ne= Integer.parseInt((String)res[0]);
                    if (ne>0) {
                        error("Se ha(n) detectado "+ne+" error(es) en la expresión tipo Polyn escrita.");
                    } else {
                        info("INFORMACIÓN: Se ha establecido correctamente el nuevo valor.");
                        this.pnlDatosVAR.setVisible(false);
                        this.pnlEditarVARPolyn.setVisible(false);
                        this.rbgTipoVariable.setDisabled(false);
                        this.rbgTipoVariable.setSelected("P");
                        this.btnVerVARS.setDisabled(false);
                        this.getSessionBean1().actualizarVARSPolyn();
                        this.tblVARS.setVisible(true);
                    }
                    for (int i= 1; i<res.length; ++i) {
                        String strRes= (String)res[i];
                        int l= strRes.length();
                        if (strRes.charAt(0)=='<'&&strRes.charAt(2)=='>'&&(strRes.charAt(1)=='E'||strRes.charAt(1)=='W')&&l>=7) {
                            char tipo= strRes.charAt(1);
                            strRes= strRes.substring(3, l-5)+".";
                            switch (tipo) {
                                case 'E':
                                case 'e':
                                    error(strRes);
                                    break;
                                case 'W':
                                case 'w':
                                    warn(strRes);
                                    break;
                                default:
                                    info(strRes);
                                    break;
                            }
                        } else {
                            info(strRes);
                        }
                    }
                } catch (NumberFormatException ex) {
                    error("ERROR: No se puede acceder al número de errores.");
                }
            }
        }
        return null;
    }

    public String btnCancelarPolyn_action() {
        this.pnlDatosVAR.setVisible(false);
        this.pnlEditarVARPolyn.setVisible(false);
        this.rbgTipoVariable.setDisabled(false);
        this.rbgTipoVariable.setSelected("P");
        this.btnVerVARS.setDisabled(false);
        this.getSessionBean1().actualizarVARSPolyn();
        this.tblVARS.setVisible(true);
        return null;
    }

    public String btnModificarVARRatio_action() {
        TOLMachine tm= this.getSessionBean1().getTm();
        TOLVariable var= this.getSessionBean1().getVarActual();
        if (var==null) {
            error("ERROR: No se puede acceder a la Variable.");
        } else {
            String code= var.getName()+":="+(String)this.txtVARRatio.getText();
            Object res[]= tm.execute(code);
            if (res==null) {
                error("ERROR: No se pudo acceder al Intérprete de TOL.");
            } else {
                try {
                    int ne= Integer.parseInt((String)res[0]);
                    if (ne>0) {
                        error("Se ha(n) detectado "+ne+" error(es) en la expresión tipo Ratio escrita.");
                    } else {
                        info("INFORMACIÓN: Se ha establecido correctamente el nuevo valor.");
                        this.pnlDatosVAR.setVisible(false);
                        this.pnlEditarVARRatio.setVisible(false);
                        this.rbgTipoVariable.setDisabled(false);
                        this.rbgTipoVariable.setSelected("A");
                        this.btnVerVARS.setDisabled(false);
                        this.getSessionBean1().actualizarVARSRatio();
                        this.tblVARS.setVisible(true);
                    }
                    for (int i= 1; i<res.length; ++i) {
                        String strRes= (String)res[i];
                        int l= strRes.length();
                        if (strRes.charAt(0)=='<'&&strRes.charAt(2)=='>'&&(strRes.charAt(1)=='E'||strRes.charAt(1)=='W')&&l>=7) {
                            char tipo= strRes.charAt(1);
                            strRes= strRes.substring(3, l-5)+".";
                            switch (tipo) {
                                case 'E':
                                case 'e':
                                    error(strRes);
                                    break;
                                case 'W':
                                case 'w':
                                    warn(strRes);
                                    break;
                                default:
                                    info(strRes);
                                    break;
                            }
                        } else {
                            info(strRes);
                        }
                    }
                } catch (NumberFormatException ex) {
                    error("ERROR: No se puede acceder al número de errores.");
                }
            }
        }
        return null;
    }

    public String btnCancelarRatio_action() {
        this.pnlDatosVAR.setVisible(false);
        this.pnlEditarVARRatio.setVisible(false);
        this.rbgTipoVariable.setDisabled(false);
        this.rbgTipoVariable.setSelected("A");
        this.btnVerVARS.setDisabled(false);
        this.getSessionBean1().actualizarVARSRatio();
        this.tblVARS.setVisible(true);
        return null;
    }

    public String btnModificarVARCode_action() {
        int lFuncion = 0;
        String funcion = (String) this.txtVARCodeName.getText();
        if (funcion != null) {
            lFuncion = funcion.length();
        }
        if (lFuncion == 0) {
            error("ERROR: Debe escribir el nuevo nombre de función a asignar a la variable.");
        } else {
                TOLVariable var= this.getSessionBean1().getVarActual();
                if (var==null) {
                    error("ERROR: No se puede acceder a la Variable.");
                } else {
                    try {
                        TOLMachine tm= this.getSessionBean1().getTm();
                        TOLCode tc = new TOLCode(funcion, null);
                        tm.setVariableValue(var.getName(), tc);
                        this.pnlDatosVAR.setVisible(false);
                        this.pnlEditarVARCode.setVisible(false);
                        this.rbgTipoVariable.setDisabled(false);
                        this.rbgTipoVariable.setSelected("F");
                        this.btnVerVARS.setDisabled(false);
                        this.getSessionBean1().actualizarVARSCode();
                        this.tblVARS.setVisible(true);
                    } catch (TOLMachineException ex) {
                        error("ERROR: " + ex.getMessage());
                    }
                }
        }
        return null;
    }

    public String btnCancelarVARCode_action() {
        this.pnlDatosVAR.setVisible(false);
        this.pnlEditarVARCode.setVisible(false);
        this.rbgTipoVariable.setDisabled(false);
        this.rbgTipoVariable.setSelected("F");
        this.btnVerVARS.setDisabled(false);
        this.getSessionBean1().actualizarVARSCode();
        this.tblVARS.setVisible(true);
        return null;
    }

    public String btnModificarVARSerie_action() {
        TOLMachine tm= this.getSessionBean1().getTm();
        TOLVariable var= this.getSessionBean1().getVarActual();
        if (var==null) {
            error("ERROR: No se puede acceder a la Variable.");
        } else {
            String code= var.getName()+":="+(String)this.txtVARSerie.getText();
            Object res[]= tm.execute(code);
            if (res==null) {
                error("ERROR: No se pudo acceder al Intérprete de TOL.");
            } else {
                try {
                    int ne= Integer.parseInt((String)res[0]);
                    if (ne>0) {
                        error("Se ha(n) detectado "+ne+" error(es) en la expresión tipo Serie escrita.");
                    } else {
                        info("INFORMACIÓN: Se ha establecido correctamente el nuevo valor.");
                        this.pnlDatosVAR.setVisible(false);
                        this.pnlEditarVARSerie.setVisible(false);
                        this.rbgTipoVariable.setDisabled(false);
                        this.rbgTipoVariable.setSelected("S");
                        this.btnVerVARS.setDisabled(false);
                        this.getSessionBean1().actualizarVARSSerie();
                        this.tblVARS.setVisible(true);
                    }
                    for (int i= 1; i<res.length; ++i) {
                        String strRes= (String)res[i];
                        int l= strRes.length();
                        if (strRes.charAt(0)=='<'&&strRes.charAt(2)=='>'&&(strRes.charAt(1)=='E'||strRes.charAt(1)=='W')&&l>=7) {
                            char tipo= strRes.charAt(1);
                            strRes= strRes.substring(3, l-5)+".";
                            switch (tipo) {
                                case 'E':
                                case 'e':
                                    error(strRes);
                                    break;
                                case 'W':
                                case 'w':
                                    warn(strRes);
                                    break;
                                default:
                                    info(strRes);
                                    break;
                            }
                        } else {
                            info(strRes);
                        }
                    }
                } catch (NumberFormatException ex) {
                    error("ERROR: No se puede acceder al número de errores.");
                }
            }
        }
        return null;
    }

    public String btnCancelarSerie_action() {
        this.pnlDatosVAR.setVisible(false);
        this.pnlEditarVARSerie.setVisible(false);
        this.rbgTipoVariable.setDisabled(false);
        this.rbgTipoVariable.setSelected("S");
        this.btnVerVARS.setDisabled(false);
        this.getSessionBean1().actualizarVARSSerie();
        this.tblVARS.setVisible(true);
        return null;
    }

    public String btnModificarVARTimeSet_action() {
        TOLMachine tm= this.getSessionBean1().getTm();
        TOLVariable var= this.getSessionBean1().getVarActual();
        if (var==null) {
            error("ERROR: No se puede acceder a la Variable.");
        } else {
            String code= var.getName()+":="+(String)this.txtVARTimeSet.getText();
            Object res[]= tm.execute(code);
            if (res==null) {
                error("ERROR: No se pudo acceder al Intérprete de TOL.");
            } else {
                try {
                    int ne= Integer.parseInt((String)res[0]);
                    if (ne>0) {
                        error("Se ha(n) detectado "+ne+" error(es) en la expresión tipo TimeSet escrita.");
                    } else {
                        info("INFORMACIÓN: Se ha establecido correctamente el nuevo valor.");
                        this.pnlDatosVAR.setVisible(false);
                        this.pnlEditarVARTimeSet.setVisible(false);
                        this.rbgTipoVariable.setDisabled(false);
                        this.rbgTipoVariable.setSelected("I");
                        this.btnVerVARS.setDisabled(false);
                        this.getSessionBean1().actualizarVARSTimeSet();
                        this.tblVARS.setVisible(true);
                    }
                    for (int i= 1; i<res.length; ++i) {
                        String strRes= (String)res[i];
                        int l= strRes.length();
                        if (strRes.charAt(0)=='<'&&strRes.charAt(2)=='>'&&(strRes.charAt(1)=='E'||strRes.charAt(1)=='W')&&l>=7) {
                            char tipo= strRes.charAt(1);
                            strRes= strRes.substring(3, l-5)+".";
                            switch (tipo) {
                                case 'E':
                                case 'e':
                                    error(strRes);
                                    break;
                                case 'W':
                                case 'w':
                                    warn(strRes);
                                    break;
                                default:
                                    info(strRes);
                                    break;
                            }
                        } else {
                            info(strRes);
                        }
                    }
                } catch (NumberFormatException ex) {
                    error("ERROR: No se puede acceder al número de errores.");
                }
            }
        }
        return null;
    }

    public String btnCancelarTimeSet_action() {
        this.pnlDatosVAR.setVisible(false);
        this.pnlEditarVARTimeSet.setVisible(false);
        this.rbgTipoVariable.setDisabled(false);
        this.rbgTipoVariable.setSelected("I");
        this.btnVerVARS.setDisabled(false);
        this.getSessionBean1().actualizarVARSTimeSet();
        this.tblVARS.setVisible(true);
        return null;
    }

    public String btnModificarVARSet_action() {
        TOLMachine tm= this.getSessionBean1().getTm();
        TOLVariable var= this.getSessionBean1().getVarActual();
        if (var==null) {
            error("ERROR: No se puede acceder a la Variable.");
        } else {
            String code= var.getName()+":="+(String)this.txtVARSet.getText();
            Object res[]= tm.execute(code);
            if (res==null) {
                error("ERROR: No se pudo acceder al Intérprete de TOL.");
            } else {
                try {
                    int ne= Integer.parseInt((String)res[0]);
                    if (ne>0) {
                        error("Se ha(n) detectado "+ne+" error(es) en la expresión tipo Set escrita.");
                    } else {
                        info("INFORMACIÓN: Se ha establecido correctamente el nuevo valor.");
                        this.pnlDatosVAR.setVisible(false);
                        this.pnlEditarVARSet.setVisible(false);
                        this.rbgTipoVariable.setDisabled(false);
                        this.rbgTipoVariable.setSelected("E");
                        this.btnVerVARS.setDisabled(false);
                        this.getSessionBean1().actualizarVARSSet();
                        this.tblVARS.setVisible(true);
                    }
                    for (int i= 1; i<res.length; ++i) {
                        String strRes= (String)res[i];
                        int l= strRes.length();
                        if (strRes.charAt(0)=='<'&&strRes.charAt(2)=='>'&&(strRes.charAt(1)=='E'||strRes.charAt(1)=='W')&&l>=7) {
                            char tipo= strRes.charAt(1);
                            strRes= strRes.substring(3, l-5)+".";
                            switch (tipo) {
                                case 'E':
                                case 'e':
                                    error(strRes);
                                    break;
                                case 'W':
                                case 'w':
                                    warn(strRes);
                                    break;
                                default:
                                    info(strRes);
                                    break;
                            }
                        } else {
                            info(strRes);
                        }
                    }
                } catch (NumberFormatException ex) {
                    error("ERROR: No se puede acceder al número de errores.");
                }
            }
        }
        return null;
    }

    public String btnCancelarSet_action() {
        this.pnlDatosVAR.setVisible(false);
        this.pnlEditarVARSet.setVisible(false);
        this.rbgTipoVariable.setDisabled(false);
        this.rbgTipoVariable.setSelected("P");
        this.btnVerVARS.setDisabled(false);
        this.getSessionBean1().actualizarVARSSet();
        this.tblVARS.setVisible(true);
        return null;
    }
}

