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
                <webuijsf:head id="head1" title="TOLJava-WebDEMO==&gt; Descargas de fuentes y binarios">
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
                                style="border-bottom: solid navy 1px; background-color: rgb(204, 204, 255); color: navy; font-size: 18px; font-variant: small-caps; font-weight: bolder; height: 28px; text-align: center; width: 100%" text="Proyecto TOLJava: Descarga de código fuente y binarios"/>
                            <h:panelGrid id="gridPanel1" style="border-top: solid navy 1px; height: 4px; width: 100%"/>
                            <webuijsf:staticText id="staticText2" style="color: navy; font-size: 12px; text-align: justify" text="En esta página puedes encontrar varios programas completos que te servirán como ejemplos de utilización de TOLJava; los programas que se muestran se irán incrementando enla misma medida en que se van desarrollando nuevas clases, interfaces o excepciones.&#xa;Para la compilación de estos ejemplos necesitarás los archivos binarios redistributiblee de TOLJava, los cuales puedes descargar usando los enlaces siguientes:"/>
                            <webuijsf:hyperlink id="hyperlink1" style="color: navy; font-size: 14px; font-weight: bolder; margin-left: 20px" target="_blank"
                                text="Descargar Binarios de TOLJava (Linux)" url="/dist/toljava-bin-linux.zip"/>
                            <webuijsf:hyperlink id="hyperlink3" style="color: navy; font-size: 14px; font-weight: bolder; margin-left: 20px" target="_blank"
                                text="Descargar Binarios de TOLJava (Windows)" url="/dist/toljava-bin-windows.zip"/>
                            <webuijsf:staticText id="staticText11" style="color: navy; font-size: 12px; text-align: justify" text="El código fuente de todos los ejemplos lo puedes descargar usando el siguiente enlace:"/>
                            <webuijsf:hyperlink id="hyperlink2" style="color: navy; font-size: 14px; font-weight: bolder; margin-left: 20px" target="_blank"
                                text="Descargar el código fuente de todos de todos los ejemplos" url="/dist/toljava-ejemplos.zip"/>
                            <webuijsf:staticText id="staticText22" style="color: navy; font-size: 12px; text-align: justify" text="El código fuente de todo TOLJava (fuentes C, fuentes Java) en proyectos para Netbeans 6.5.1 (proyectos: TOLJava, TOLJavaJNI, TOLJava-Ejemplos y WebTOLJavaDEMO) lo puedes descargar en el enlace siguiente:"/>
                            <webuijsf:hyperlink id="hyperlink4" style="color: navy; font-size: 14px; font-weight: bolder; margin-left: 20px"
                                text="Descargar el código fuente de TOLJava como proyectos para Netbeans 6.5.1" url="/dist/toljava-netbeans.zip"/>
                        </h:panelGrid>
                    </webuijsf:form>
                </webuijsf:body>
            </webuijsf:html>
        </webuijsf:page>
    </f:view>
</jsp:root>
