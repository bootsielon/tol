<?xml version="1.0" encoding="UTF-8"?>
<!-- 
    Document   : Docs
    Created on : Nov 28, 2010, 1:55:24 PM
    Author     : hcarralero
-->
<jsp:root version="2.1" xmlns:f="http://java.sun.com/jsf/core" xmlns:h="http://java.sun.com/jsf/html" xmlns:jsp="http://java.sun.com/JSP/Page" xmlns:webuijsf="http://www.sun.com/webui/webuijsf">
    <jsp:directive.page contentType="text/html;charset=UTF-8" pageEncoding="UTF-8"/>
    <f:view>
        <webuijsf:page id="page1">
            <webuijsf:html id="html1">
                <webuijsf:head id="head1" title="TOLJava-WebDEMO==&gt; Documentación">
                    <webuijsf:link id="link1" url="/resources/stylesheet.css"/>
                    <webuijsf:script type="text/javascript" url="sh/scripts/shCore.js"/>
                    <webuijsf:script type="text/javascript" url="sh/scripts/shBrushJava.js"/>
                    <webuijsf:link id="link2" rel="stylesheet" type="text/css" url="sh/styles/shCore.css"/>
                    <webuijsf:link id="link3" rel="stylesheet" type="text/css" url="sh/styles/shThemeDefault.css"/>
                </webuijsf:head>
                <webuijsf:body id="body1" style="background-color: rgb(224, 224, 255); -rave-layout: grid">
                    <webuijsf:form id="form1">
                        <div style="left: 0px; top: 0px; position: absolute; width: 100%">
                            <jsp:directive.include file="Encabezado.jspf"/>
                        </div>
                        <div style="left: 2px; top: 68px; position: absolute; width: 200px">
                            <jsp:directive.include file="MenuIzquierdo.jspf"/>
                        </div>
                        <h:panelGrid id="pnlDocs" style="border: 1px solid navy; left: 202px; top: 68px; position: absolute">
                            <webuijsf:staticText id="staticText1"
                                style="border-bottom: solid navy 1px; background-color: rgb(204, 204, 255); color: navy; font-size: 18px; font-variant: small-caps; font-weight: bolder; height: 28px; text-align: center; width: 100%" text="Intérprete de TOLJava"/>
                            <webuijsf:tabSet id="tsInterprete" selected="tabCodigoFuente">
                                <webuijsf:tab id="tabCodigoFuente" text="Intérprete">
                                    <h:panelGrid id="gridPanel2" style="border: 1px solid teal; height: 32px; text-align: left; width: 100%">
                                        <webuijsf:staticText id="staticText3"
                                            style="border-bottom: solid navy 2px; background-color: rgb(155, 230, 251); color: navy; font-size: 14px; font-variant: small-caps; font-weight: bolder; height: 21px; text-align: left; width: 100%" text="Panel de Ejecución de Código TOL"/>
                                        <webuijsf:label id="label1" text="Escriba el CÓDIGO TOL que desea EJECUTAR:"/>
                                        <webuijsf:textArea binding="#{Interprete.taCodigo}" columns="96" id="taCodigo" rows="8"/>
                                        <webuijsf:button actionExpression="#{Interprete.btnEjecutarCodigo_action}" id="btnEjecutarCodigo" text="Ejecutar el Código"/>
                                        <webuijsf:staticText binding="#{Interprete.stxtResultadoEjecutar}" escape="false" id="stxtResultadoEjecutar"/>
                                    </h:panelGrid>
                                </webuijsf:tab>
                                <webuijsf:tab id="tabCodigo" text="Código Fuente">
                                    <h:panelGrid id="gridPanel1" style="border: 1px solid teal; height: 32px; text-align: left; width: 100%">
                                        <webuijsf:staticText id="staticText2"
                                            style="border-bottom: solid navy 2px; background-color: rgb(155, 230, 251); color: navy; font-size: 14px; font-variant: small-caps; font-weight: bolder; height: 21px; text-align: left; width: 100%" text="Panel de Revisión del Código Fuente"/>
                                        <pre class="brush: java; highlight:[2, 5, 10]; toolbar: false;">
                                            //Se deben importar las clases a usar del paquete de TOLJava
                                            import com.hcarralero.toljava.TOLMachine;
                                            ...
                                            //Se crea una instancia de TOLMachine, la cual nos sirve para acceder al Intérprete de TOL
                                            TOLMachine tm= new TOLMachine();
                                            //Usamos una cadena para almacenar el código TOL a ejecutar
                                            //En este caso la variable 'code' toma el código ingresado por el usuario en la caja de texto
                                            String code= (String)this.taExpDemo01.getText();
                                            //Se ejecuta el código ingresado. El resultado es un arreglo de cadenas Object
                                            res[]= tm.execute(code);
                                            //La primera cadena representa el número de errores detectado por el Intérprete de TOL
                                            String strNE= (String)res[0]; try {
                                                //Usamos un método para convertir la cadena en un número
                                                //ne contendrá el número de errores, 0 significa que no hubo errores
                                                int ne= Integer.parse(strNE);
                                                //El resto de los elementos son los mensajes producidos por el Intérprete de TOL
                                                //Los mensajes
                                            } catch (NumberFormatException ex) {
                                                //Si se llega a este punto se habrá producido un error inesperado
                                            }
                                        </pre>
                                        <webuijsf:script type="text/javascript">
                                            SyntaxHighlighter.all()
                                        </webuijsf:script>
                                    </h:panelGrid>
                                </webuijsf:tab>
                            </webuijsf:tabSet>
                        </h:panelGrid>
                    </webuijsf:form>
                </webuijsf:body>
            </webuijsf:html>
        </webuijsf:page>
    </f:view>
</jsp:root>
