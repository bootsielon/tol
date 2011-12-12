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
                <webuijsf:head id="head1" title="TOLJava-WebDEMO==&gt; Inicio">
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
                        <h:panelGrid id="pnlDocs" style="border: 1px solid navy; left: 202px; top: 68px; position: absolute">
                            <webuijsf:staticText id="staticText1"
                                style="border-bottom: solid navy 1px; background-color: rgb(204, 204, 255); color: navy; font-size: 18px; font-variant: small-caps; font-weight: bolder; height: 28px; text-align: center; width: 100%" text="Proyecto TOLJava: Biblioteca de Acceso a TOL desde Java"/>
                            <h:panelGrid id="gridPanel1" style="border-top: solid navy 1px; height: 4px; width: 100%"/>
                            <webuijsf:staticText id="staticText2" style="color: navy; font-size: 12px; text-align: justify" text="TOLJava es una biblioteca que permite el acceso a una instancia de TOL desde programas escritos en Java. Para facilitar el uso de esta biblioteca hemos elaborado una Aplicación Web Demo, a la cual estás accediendo en este momento.&#xa;La DEMO consta de esta Página de Inicio, un conjunto de programas demostrativos y la documentación que está orientada a los desarrolladores; a cada una de estas opciones puedes acceder por medio del &quot;Menú Principal&quot; que está a la izquierda de todas las páginas.&#xa;Cada programa demostrativo consta de dos pestañas, en la primera puedes acceder a la ejecución del programa y en la segunda encontrarás síntesis del código (con abundantes comentarios) fuente que se usa en el programa."/>
                        </h:panelGrid>
                    </webuijsf:form>
                </webuijsf:body>
            </webuijsf:html>
        </webuijsf:page>
    </f:view>
</jsp:root>
