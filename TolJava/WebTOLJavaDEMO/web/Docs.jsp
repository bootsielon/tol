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
                <webuijsf:head id="head1" title="TOLJava==&gt;Documentación">
                    <webuijsf:link id="link1" url="/resources/stylesheet.css"/>
                </webuijsf:head>
                <webuijsf:body id="body1" style="background-color: rgb(224, 224, 255); -rave-layout: grid">
                    <webuijsf:form id="form1">
                        <div style="left: 0px; top: 0px; position: absolute; width: 100%">
                            <jsp:directive.include file="Encabezado.jspf"/>
                        </div>
                        <div style="left: 2px; top: 68px; position: absolute; width: 200px">
                            <jsp:directive.include file="MenuIzquierdo.jspf"/>
                        </div>
                        <h:panelGrid id="pnlDocs" style="border: 1px solid navy; left: 216px; top: 72px; position: absolute">
                            <webuijsf:staticText id="staticText1"
                                style="border-bottom: solid navy 1px; background-color: rgb(204, 204, 255); color: navy; font-size: 18px; font-variant: small-caps; font-weight: bolder; height: 28px; text-align: center; width: 100%" text="Documentación de TOLJava"/>
                            <webuijsf:staticText id="staticText2" style="color: navy" text="La documentación que se ha generado hasta este momento para TOLJava es la siguiente:"/>
                            <webuijsf:panelLayout id="layoutPanel2" panelLayout="flow" style="height: 22px; width: 100%">
                                <webuijsf:staticText id="staticText5" style="font-size: 14px; font-weight: bolder; margin-right: 4px" text="1)"/>
                                <webuijsf:hyperlink id="hyperlink2" style="font-size: 14px; font-weight: bolder" target="_blank"
                                    text="Guía de instalación y primeros pasos" url="/docs/toljava-guia-instalacion.pdf"/>
                            </webuijsf:panelLayout>
                            <webuijsf:staticText id="staticText6" style="color: navy" text="En este manual se describe todo el proceso de instalación de TOLJava, así como las pruebas iniciales de funcionamiento. "/>
                            <webuijsf:panelLayout id="layoutPanel1" panelLayout="flow" style="height: 22px; width: 100%">
                                <webuijsf:staticText id="staticText3" style="font-size: 14px; font-weight: bolder; margin-right: 4px" text="2)"/>
                                <webuijsf:hyperlink actionExpression="#{Docs.hyperlink1_action}" id="hyperlink1" style="font-size: 14px; font-weight: bolder"
                                    target="_blank" text="Manual del Programador" type="application/pdf" url="/docs/toljava-guiaprogramador.pdf"/>
                            </webuijsf:panelLayout>
                            <webuijsf:staticText id="staticText4" style="color: navy" text="Este manual consta de dos partes, en la primera se describe todo el proceso de instalación de TOLJava, así como las pruebas iniciales de funcionamiento. En la segunda parte se describen todos los procesos requeridos para usar en una aplicación Java la biblioteca TOLJava. Se describen los procesos típicos requeridos y se hace la disección de ejemplos completos de aplicación. Este manual es indispensable para todo programador que quiera acceder a TOL desde Java."/>
                            <webuijsf:panelLayout id="layoutPanel3" panelLayout="flow" style="height: 22px; width: 100%">
                                <webuijsf:staticText id="staticText7" style="font-size: 14px; font-weight: bolder; margin-right: 4px" text="2)"/>
                                <webuijsf:hyperlink actionExpression="#{Docs.hyperlink3_action}" id="hyperlink3" style="font-size: 14px; font-weight: bolder"
                                    target="_blank" text="Referencia del API de TOLJava" type="application/pdf" url="/docs/javadoc/index.html"/>
                            </webuijsf:panelLayout>
                            <webuijsf:staticText id="staticText8" style="color: navy; text-align: justify" text="Esta referencia usa las convenciones de la documentación de Javadoc para exponer todo el API de TOLJava. Este documento es la referencia definitiva para que los programadores consulten el contenido del paquete com.hcarralero.toljava; aquí se encuentran las especificaciones de todas las interfaces y clases que están incluidas en este paquete."/>
                        </h:panelGrid>
                    </webuijsf:form>
                </webuijsf:body>
            </webuijsf:html>
        </webuijsf:page>
    </f:view>
</jsp:root>
