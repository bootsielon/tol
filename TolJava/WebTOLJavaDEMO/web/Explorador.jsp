<?xml version="1.0" encoding="UTF-8"?>
<!-- 
    Document   : Page1
    Created on : Nov 28, 2010, 5:30:40 AM
    Author     : hcarralero
-->
<jsp:root version="2.1" xmlns:f="http://java.sun.com/jsf/core" xmlns:h="http://java.sun.com/jsf/html" xmlns:jsp="http://java.sun.com/JSP/Page" xmlns:webuijsf="http://www.sun.com/webui/webuijsf">
    <jsp:directive.page contentType="text/html;charset=UTF-8" pageEncoding="UTF-8"/>
    <f:view>
        <webuijsf:page id="page1">
            <webuijsf:html id="html1">
                <webuijsf:head id="head1" title="TOLJava-WebDEMO==&gt; Explorador de Variables">
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
                            <h:panelGrid id="gridPanel3"/>
                        </div>
                        <h:panelGrid id="pnlExplorador" style="border: 1px solid navy; left: 202px; top: 68px; position: absolute">
                            <webuijsf:staticText id="staticText1"
                                style="border-bottom: solid navy 1px; background-color: rgb(204, 204, 255); color: navy; font-size: 18px; font-variant: small-caps; font-weight: bolder; height: 28px; text-align: center; width: 100%" text="Explorador de Variables"/>
                            <webuijsf:tabSet id="tsDemo" selected="tabVariables" style="border: 1px solid navy; width: 766px">
                                <webuijsf:tab id="tabVariables" text="Variables">
                                    <h:panelGrid id="gridPanel1" style="width: 100%">
                                        <webuijsf:staticText id="staticText2"
                                            style="background-color: rgb(155, 230, 251); color: navy; font-size: 14px; font-variant: small-caps; font-weight: bolder; height: 21px; text-align: left; width: 100%" text="Panel para la Exploración de las Variables de TOL"/>
                                        <webuijsf:messageGroup id="messageGroup1" title="Mensajes del Sistema"/>
                                        <h:panelGrid columns="3" id="gridPanel5" style="border-top: solid navy 1px; border-bottom: solid navy 1px; height: 32px; width: 100%">
                                            <webuijsf:label id="label1" style="text-align: right; width: 100%" text="Tipo de Variable:"/>
                                            <webuijsf:radioButtonGroup binding="#{Explorador.rbgTipoVariable}" columns="6" id="rbgTipoVariable"
                                                items="#{Explorador.rbgTipoVariableDefaultOptions.options}" onClick="" style="width: 455px"/>
                                            <webuijsf:button actionExpression="#{Explorador.btnVerVARS_action}" binding="#{Explorador.btnVerVARS}"
                                                id="btnVerVARS" text="Ver Variables" toolTip="Ver las variables del tipo seleccionado"/>
                                        </h:panelGrid>
                                        <webuijsf:table augmentTitle="false" binding="#{Explorador.tblVARS}" id="tblVARS" paginateButton="true"
                                            paginationControls="true" width="100%">
                                            <webuijsf:tableRowGroup emptyDataMsg="No hay variables para mostrar" id="tableRowGroup1" rows="10"
                                                sourceData="#{SessionBean1.vars}" sourceVar="currentRow">
                                                <webuijsf:tableColumn headerText="Nombre" id="tableColumn1" sort="name" width="120px">
                                                    <webuijsf:staticText id="staticText3" text="#{currentRow.value['name']}"/>
                                                </webuijsf:tableColumn>
                                                <webuijsf:tableColumn headerText="Valor" id="tableColumn2">
                                                    <webuijsf:staticText id="staticText4" text="#{Explorador.VARValor}"/>
                                                </webuijsf:tableColumn>
                                                <webuijsf:tableColumn headerText="Descripción" id="tableColumn3" sort="desc">
                                                    <webuijsf:staticText id="staticText5" text="#{currentRow.value['desc']}"/>
                                                </webuijsf:tableColumn>
                                                <webuijsf:tableColumn align="center" id="tableColumn4" valign="middle" width="60px">
                                                    <webuijsf:button actionExpression="#{Explorador.btnEditarVAR_action}" id="btnEditarVAR" text="Editar" toolTip="Cambiar el valor de la variable de esta fila"/>
                                                </webuijsf:tableColumn>
                                            </webuijsf:tableRowGroup>
                                        </webuijsf:table>
                                        <webuijsf:panelLayout binding="#{Explorador.pnlDatosVAR}" id="pnlDatosVAR"
                                            style="border-top: solid navy 1px; border-bottom: solid navy 1px; height: 238px; position: relative; width: 100%; -rave-layout: grid" visible="false">
                                            <webuijsf:staticText id="staticText6"
                                                style="border-bottom: solid navy 3px; background-color: rgb(160, 194, 214); color: navy; font-size: 18px; font-variant: small-caps; font-weight: bolder; height: 28px; left: 0px; top: 0px; position: absolute; text-align: left; width: 100%" text="Datos Generales de la Variable"/>
                                            <webuijsf:label id="label2" style="left: 24px; top: 48px; position: absolute; text-align: left" text="Nombre:"/>
                                            <webuijsf:textField binding="#{Explorador.txtVARName}" columns="96" disabled="true" id="txtVARName" style="left: 24px; top: 72px; position: absolute"/>
                                            <webuijsf:label id="label3" style="left: 24px; top: 96px; position: absolute" text="Descripción:"/>
                                            <webuijsf:textArea binding="#{Explorador.taVARDesc}" columns="96" disabled="true" id="taVARDesc" rows="5" style="left: 24px; top: 120px; position: absolute"/>
                                        </webuijsf:panelLayout>
                                        <webuijsf:panelLayout binding="#{Explorador.pnlEditarVARText}" id="pnlEditarVARText"
                                            style="border-top: solid navy 1px; border-bottom: solid navy 1px; height: 238px; position: relative; width: 100%; -rave-layout: grid" visible="false">
                                            <webuijsf:staticText id="staticText7"
                                                style="border-bottom: solid navy 3px; background-color: rgb(160, 194, 214); color: navy; font-size: 18px; font-variant: small-caps; font-weight: bolder; height: 28px; left: 0px; top: 0px; position: absolute; text-align: left; width: 100%" text="Datos Modificables de la Variable Tipo Text"/>
                                            <webuijsf:label id="label4" style="left: 24px; top: 48px; position: absolute; text-align: left" text="Texto actual/Nuevo Texto:"/>
                                            <webuijsf:textArea binding="#{Explorador.taVARTextText}" columns="96" id="taVARTextText" rows="5" style="left: 24px; top: 72px; position: absolute"/>
                                            <webuijsf:panelLayout id="layoutPanel1" panelLayout="flow" style="border-width: 1px; border-style: dashed; background-color: rgb(130, 154, 179); height: 32px; left: 20%; top: 192px; position: absolute; text-align: center; width: 60%">
                                                <webuijsf:button actionExpression="#{Explorador.btnModificarVARText_action}" id="btnModificarVARText"
                                                    style="height: 24px; margin-right: 8px; margin-top: 4px; width: 120px" text="Modificar Valor"/>
                                                <webuijsf:button actionExpression="#{Explorador.btnCancelarVARText_action}" id="btnCancelarVARText"
                                                    style="height: 24px; margin-left: 8px; margin-top: 4px; width: 120px" text="Cancelar"/>
                                            </webuijsf:panelLayout>
                                        </webuijsf:panelLayout>
                                        <webuijsf:panelLayout binding="#{Explorador.pnlEditarVARReal}" id="pnlEditarVARReal"
                                            style="border-top: solid navy 1px; border-bottom: solid navy 1px; height: 190px; position: relative; width: 100%; -rave-layout: grid" visible="false">
                                            <webuijsf:staticText id="staticText8"
                                                style="border-bottom: solid navy 3px; background-color: rgb(160, 194, 214); color: navy; font-size: 18px; font-variant: small-caps; font-weight: bolder; height: 28px; left: 0px; top: 0px; position: absolute; text-align: left; width: 100%" text="Datos Modificables de la Variable Tipo Real"/>
                                            <webuijsf:label id="label5" style="left: 24px; top: 48px; position: absolute" text="Valor actual/Nuevo Valor:"/>
                                            <webuijsf:textField binding="#{Explorador.txtVARRealValue}" id="txtVARRealValue" style="left: 24px; top: 72px; position: absolute"/>
                                            <webuijsf:panelLayout id="layoutPanel2" panelLayout="flow" style="border-width: 1px; border-style: dashed; background-color: rgb(130, 154, 179); height: 32px; left: 20%; top: 144px; position: absolute; text-align: center; width: 60%">
                                                <webuijsf:button actionExpression="#{Explorador.btnModificarVARReal_action}" id="btnModificarVARReal"
                                                    style="height: 24px; margin-right: 8px; margin-top: 4px; width: 120px" text="Modificar Valor"/>
                                                <webuijsf:button actionExpression="#{Explorador.btnCancelarVARReal_action}" id="btnCancelarVARReal"
                                                    style="height: 24px; margin-left: 8px; margin-top: 4px; width: 120px" text="Cancelar"/>
                                            </webuijsf:panelLayout>
                                        </webuijsf:panelLayout>
                                        <webuijsf:panelLayout binding="#{Explorador.pnlEditarVARDate}" id="pnlEditarVARDate"
                                            style="border-top: solid navy 1px; border-bottom: solid navy 1px; height: 190px; position: relative; width: 100%; -rave-layout: grid" visible="false">
                                            <webuijsf:staticText id="staticText9"
                                                style="border-bottom: solid navy 3px; background-color: rgb(160, 194, 214); color: navy; font-size: 18px; font-variant: small-caps; font-weight: bolder; height: 28px; left: 0px; top: 0px; position: absolute; text-align: left; width: 100%" text="Datos Modificables de la Variable Tipo Date"/>
                                            <webuijsf:label id="label6" style="position: absolute; left: 24px; top: 48px" text="Fecha actual/Nuevo Fecha:"/>
                                            <webuijsf:calendar binding="#{Explorador.calVARDateDate}" dateFormatPattern="dd-MM-yyyy"
                                                dateFormatPatternHelp="dd-MM-aaaa" id="calVARDateDate" style="position: absolute; left: 24px; top: 72px"/>
                                            <webuijsf:panelLayout id="layoutPanel3" panelLayout="flow" style="border-width: 1px; border-style: dashed; background-color: rgb(130, 154, 179); height: 32px; left: 20%; top: 144px; position: absolute; text-align: center; width: 60%">
                                                <webuijsf:button actionExpression="#{Explorador.btnModificarVARDate_action}" id="btnModificarVARDate"
                                                    style="height: 24px; margin-right: 8px; margin-top: 4px; width: 120px" text="Modificar Valor"/>
                                                <webuijsf:button actionExpression="#{Explorador.btnCancelarVARDate_action}" id="btnCancelarVARDate"
                                                    style="height: 24px; margin-left: 8px; margin-top: 4px; width: 120px" text="Cancelar"/>
                                            </webuijsf:panelLayout>
                                        </webuijsf:panelLayout>
                                        <webuijsf:panelLayout binding="#{Explorador.pnlEditarVARComplex}" id="pnlEditarVARComplex"
                                            style="border-top: solid navy 1px; border-bottom: solid navy 1px; height: 190px; position: relative; width: 100%; -rave-layout: grid" visible="false">
                                            <webuijsf:staticText id="staticText10"
                                                style="border-bottom: solid navy 3px; background-color: rgb(160, 194, 214); color: navy; font-size: 18px; font-variant: small-caps; font-weight: bolder; height: 28px; left: 0px; top: 0px; position: absolute; text-align: left; width: 100%" text="Datos Modificables de la Variable Tipo Complex"/>
                                            <webuijsf:label id="label7" style="left: 96px; top: 48px; position: absolute" text="Valor de X actual/Nuevo Valor de X:"/>
                                            <webuijsf:textField binding="#{Explorador.txtVARComplexX}" id="txtVARComplexX" style="left: 96px; top: 72px; position: absolute"/>
                                            <webuijsf:label id="label8" style="left: 408px; top: 48px; position: absolute" text="Valor de Y actual/Nuevo Valor de Y:"/>
                                            <webuijsf:textField binding="#{Explorador.txtVARComplexY}" id="txtVARComplexY" style="left: 408px; top: 72px; position: absolute"/>
                                            <webuijsf:panelLayout id="layoutPanel4" panelLayout="flow" style="border-width: 1px; border-style: dashed; background-color: rgb(130, 154, 179); height: 32px; left: 20%; top: 144px; position: absolute; text-align: center; width: 60%">
                                                <webuijsf:button actionExpression="#{Explorador.btnModificarVARComplex_action}" id="btnModificarVARComplex"
                                                    style="height: 24px; margin-right: 8px; margin-top: 4px; width: 120px" text="Modificar Valor"/>
                                                <webuijsf:button actionExpression="#{Explorador.btnCancelarVARComplex_action}" id="btnCancelarVARComplex"
                                                    style="height: 24px; margin-left: 8px; margin-top: 4px; width: 120px" text="Cancelar"/>
                                            </webuijsf:panelLayout>
                                        </webuijsf:panelLayout>
                                        <webuijsf:panelLayout binding="#{Explorador.pnlEditarVARMatrix}" id="pnlEditarVARMatrix"
                                            style="border-top: solid navy 1px; border-bottom: solid navy 1px; height: 94px; position: relative; width: 100%; -rave-layout: grid" visible="false">
                                            <webuijsf:staticText id="staticText12"
                                                style="border-bottom: solid navy 3px; background-color: rgb(160, 194, 214); color: navy; font-size: 18px; font-variant: small-caps; font-weight: bolder; height: 28px; left: 0px; top: 0px; position: absolute; text-align: left; width: 100%" text="Datos Modificables para la Variable Tipo Matrix"/>
                                            <webuijsf:panelLayout binding="#{Explorador.pnlAccionesVARMatrix}" id="pnlAccionesVARMatrix" panelLayout="flow" style="border-width: 1px; border-style: dashed; background-color: rgb(130, 154, 179); height: 32px; left: 20%; top: 48px; position: absolute; text-align: center; width: 60%">
                                                <webuijsf:button actionExpression="#{Explorador.btnModificarVARMatrix_action}" id="btnModificarVARMatrix"
                                                    style="height: 24px; margin-right: 8px; margin-top: 4px; width: 120px" text="Modifcar Valor"/>
                                                <webuijsf:button actionExpression="#{Explorador.btnCancelarVARMatrix_action}" id="btnCancelarVARMatrix"
                                                    style="height: 24px; margin-left: 8px; margin-top: 4px; width: 120px" text="Cancelar"/>
                                            </webuijsf:panelLayout>
                                        </webuijsf:panelLayout>
                                        <webuijsf:panelLayout binding="#{Explorador.pnlEditarVARVMatrix}" id="pnlEditarVARVMatrix"
                                            style="border-top: solid navy 1px; border-bottom: solid navy 1px; height: 94px; position: relative; width: 100%; -rave-layout: grid" visible="false">
                                            <webuijsf:staticText id="staticText13"
                                                style="border-bottom: solid navy 3px; background-color: rgb(160, 194, 214); color: navy; font-size: 18px; font-variant: small-caps; font-weight: bolder; height: 28px; left: 0px; top: 0px; position: absolute; text-align: left; width: 100%" text="Datos Modificables para la Variable tipo VMatrix"/>
                                            <webuijsf:panelLayout binding="#{Explorador.pnlAccionesVARVMatrix}" id="pnlAccionesVARVMatrix" panelLayout="flow" style="border-width: 1px; border-style: dashed; background-color: rgb(130, 154, 179); height: 32px; left: 20%; top: 48px; position: absolute; text-align: center; width: 60%">
                                                <webuijsf:button actionExpression="#{Explorador.btnModificarVARVMatrix_action}" id="btnModificarVARVMatrix"
                                                    style="height: 24px; margin-right: 8px; margin-top: 4px; width: 120px" text="Modificar Valor"/>
                                                <webuijsf:button actionExpression="#{Explorador.btnCancelarVARVMatrix_action}" id="btnCancelarVARVMatrix"
                                                    style="height: 24px; margin-left: 8px; margin-top: 4px; width: 120px" text="Cancelar"/>
                                            </webuijsf:panelLayout>
                                        </webuijsf:panelLayout>
                                        <webuijsf:panelLayout binding="#{Explorador.pnlEditarVARPolyn}" id="pnlEditarVARPolyn"
                                            style="border-top: solid navy 1px; border-bottom: solid navy 1px; height: 166px; position: relative; width: 100%; -rave-layout: grid" visible="false">
                                            <webuijsf:staticText id="staticText14"
                                                style="border-bottom: solid navy 3px; background-color: rgb(160, 194, 214); color: navy; font-size: 18px; font-variant: small-caps; font-weight: bolder; height: 28px; left: 0px; top: 0px; position: absolute; text-align: left; width: 100%" text="Datos Modificables para la Variable de Tipo Polyn"/>
                                            <webuijsf:panelLayout binding="#{Explorador.layoutPanel5}" id="layoutPanel5" panelLayout="flow" style="border-width: 1px; border-style: dashed; background-color: rgb(130, 154, 179); height: 32px; left: 20%; top: 120px; position: absolute; text-align: center; width: 60%">
                                                <webuijsf:button actionExpression="#{Explorador.btnModificarVARPolyn_action}" id="btnModificarVARPolyn"
                                                    style="height: 24px; margin-right: 8px; margin-top: 4px; width: 120px" text="Modificar Valor"/>
                                                <webuijsf:button actionExpression="#{Explorador.btnCancelarPolyn_action}" id="btnCancelarPolyn"
                                                    style="height: 24px; margin-left: 8px; margin-top: 4px; width: 120px" text="Cancelar"/>
                                            </webuijsf:panelLayout>
                                            <webuijsf:label id="label9" style="left: 0px; top: 48px; position: absolute; text-align: center; width: 100%" text="Definición en Lenguaje TOL:"/>
                                            <webuijsf:textField binding="#{Explorador.txtVARPolyn}" columns="94" id="txtVARPolyn" style="height: 32px; left: 48px; top: 72px; position: absolute; width: 90%"/>
                                        </webuijsf:panelLayout>
                                        <webuijsf:panelLayout binding="#{Explorador.pnlEditarVARRatio}" id="pnlEditarVARRatio"
                                            style="border-top: solid navy 1px; border-bottom: solid navy 1px; height: 166px; position: relative; width: 100%; -rave-layout: grid" visible="false">
                                            <webuijsf:staticText id="staticText15"
                                                style="border-bottom: solid navy 3px; background-color: rgb(160, 194, 214); color: navy; font-size: 18px; font-variant: small-caps; font-weight: bolder; height: 28px; left: 0px; top: 0px; position: absolute; text-align: left; width: 100%" text="Datos Modificables para la Variable de Tipo Ratio"/>
                                            <webuijsf:panelLayout id="layoutPanel6" panelLayout="flow" style="border-width: 1px; border-style: dashed; background-color: rgb(130, 154, 179); height: 32px; left: 20%; top: 120px; position: absolute; text-align: center; width: 60%">
                                                <webuijsf:button actionExpression="#{Explorador.btnModificarVARRatio_action}" id="btnModificarVARRatio"
                                                    style="height: 24px; margin-right: 8px; margin-top: 4px; width: 120px" text="Modificar Valor"/>
                                                <webuijsf:button actionExpression="#{Explorador.btnCancelarRatio_action}" id="btnCancelarRatio"
                                                    style="height: 24px; margin-left: 8px; margin-top: 4px; width: 120px" text="Cancelar"/>
                                            </webuijsf:panelLayout>
                                            <webuijsf:label id="label10" style="left: 0px; top: 48px; position: absolute; text-align: center; width: 100%" text="Definición en Lenguaje TOL:"/>
                                            <webuijsf:textField binding="#{Explorador.txtVARRatio}" columns="94" id="txtVARRatio" style="height: 32px; left: 48px; top: 72px; position: absolute; width: 90%"/>
                                        </webuijsf:panelLayout>
                                        <webuijsf:panelLayout binding="#{Explorador.pnlEditarVARCode}" id="pnlEditarVARCode"
                                            style="border-top: solid navy 1px; border-bottom: solid navy 1px; height: 166px; position: relative; width: 100%; -rave-layout: grid" visible="false">
                                            <webuijsf:staticText id="staticText16"
                                                style="border-bottom: solid navy 3px; background-color: rgb(160, 194, 214); color: navy; font-size: 18px; font-variant: small-caps; font-weight: bolder; height: 28px; left: 0px; top: 0px; position: absolute; text-align: left; width: 100%" text="Datos Modificables de la Variable Tipo Code"/>
                                            <webuijsf:label id="label11" style="left: 48px; top: 48px; position: absolute; text-align: right; width: 94px" text="Nombre:"/>
                                            <webuijsf:textField binding="#{Explorador.txtVARCodeName}" columns="40" id="txtVARCodeName" style="position: absolute; left: 144px; top: 48px"/>
                                            <webuijsf:label id="label12" style="left: 48px; top: 72px; position: absolute; text-align: right; width: 96px" text="Información:"/>
                                            <webuijsf:textField binding="#{Explorador.txtVARCodeInfo}" columns="80" disabled="true" id="txtVARCodeInfo" style="left: 144px; top: 72px; position: absolute"/>
                                            <webuijsf:panelLayout id="layoutPanel7" panelLayout="flow" style="border-width: 1px; border-style: dashed; background-color: rgb(130, 154, 179); height: 32px; left: 20%; top: 120px; position: absolute; text-align: center; width: 60%">
                                                <webuijsf:button actionExpression="#{Explorador.btnModificarVARCode_action}" id="btnModificarVARCode"
                                                    style="height: 24px; margin-right: 8px; margin-top: 4px; width: 120px" text="Modificar Valor"/>
                                                <webuijsf:button actionExpression="#{Explorador.btnCancelarVARCode_action}" id="btnCancelarVARCode"
                                                    style="height: 24px; margin-left: 8px; margin-top: 4px; width: 120px" text="Cancelar"/>
                                            </webuijsf:panelLayout>
                                        </webuijsf:panelLayout>
                                        <webuijsf:panelLayout binding="#{Explorador.pnlEditarVARSerie}" id="pnlEditarVARSerie"
                                            style="style&#x9;border-top: solid navy 1px; border-bottom: solid navy 1px; height: 166px; position: relative; width: 100%; -rave-layout: grid" visible="false">
                                            <webuijsf:staticText id="staticText17"
                                                style="border-bottom: solid navy 3px; background-color: rgb(160, 194, 214); color: navy; font-size: 18px; font-variant: small-caps; font-weight: bolder; height: 28px; left: 0px; top: 0px; position: absolute; text-align: left; width: 100%" text="Datos Modificables para la Variable de Tipo Serie"/>
                                            <webuijsf:label id="label13" style="left: 0px; top: 48px; position: absolute; text-align: center; width: 100%" text="Definición en Lenguaje TOL"/>
                                            <webuijsf:textField binding="#{Explorador.txtVARSerie}" columns="94" id="txtVARSerie" style="height: 32px; left: 48px; top: 72px; position: absolute; width: 90%"/>
                                            <webuijsf:panelLayout id="layoutPanel8" panelLayout="flow" style="border-width: 1px; border-style: dashed; background-color: rgb(130, 154, 179); height: 32px; left: 20%; top: 120px; position: absolute; text-align: center; width: 60%">
                                                <webuijsf:button actionExpression="#{Explorador.btnModificarVARSerie_action}" id="btnModificarVARSerie"
                                                    style="height: 24px; margin-right: 8px; margin-top: 4px; width: 120px" text="Modificar Valor"/>
                                                <webuijsf:button actionExpression="#{Explorador.btnCancelarSerie_action}" id="btnCancelarSerie"
                                                    style="height: 24px; margin-left: 8px; margin-top: 4px; width: 120px" text="Cancelar"/>
                                            </webuijsf:panelLayout>
                                        </webuijsf:panelLayout>
                                        <webuijsf:panelLayout binding="#{Explorador.pnlEditarVARTimeSet}" id="pnlEditarVARTimeSet"
                                            style="style&#x9;border-top: solid navy 1px; border-bottom: solid navy 1px; height: 166px; position: relative; width: 100%; -rave-layout: grid" visible="false">
                                            <webuijsf:staticText id="staticText18"
                                                style="border-bottom: solid navy 3px; background-color: rgb(160, 194, 214); color: navy; font-size: 18px; font-variant: small-caps; font-weight: bolder; height: 28px; left: 0px; top: 0px; position: absolute; text-align: left; width: 100%" text="Datos Modificables para la Variable tipo TimeSet"/>
                                            <webuijsf:label id="label14" style="left: 0px; top: 48px; position: absolute; text-align: center; width: 100%" text="Definición en Lenguaje TOL"/>
                                            <webuijsf:textField binding="#{Explorador.txtVARTimeSet}" columns="94" id="txtVARTimeSet" style="height: 32px; left: 48px; top: 72px; position: absolute; width: 90%"/>
                                            <webuijsf:panelLayout id="layoutPanel9" panelLayout="flow" style="border-width: 1px; border-style: dashed; background-color: rgb(130, 154, 179); height: 32px; left: 20%; top: 120px; position: absolute; text-align: center; width: 60%">
                                                <webuijsf:button actionExpression="#{Explorador.btnModificarVARTimeSet_action}" id="btnModificarVARTimeSet"
                                                    style="height: 24px; margin-right: 8px; margin-top: 4px; width: 120px" text="Modificar Valor"/>
                                                <webuijsf:button actionExpression="#{Explorador.btnCancelarTimeSet_action}" id="btnCancelarTimeSet"
                                                    style="height: 24px; margin-left: 8px; margin-top: 4px; width: 120px" text="Cancelar"/>
                                            </webuijsf:panelLayout>
                                        </webuijsf:panelLayout>
                                        <webuijsf:panelLayout binding="#{Explorador.pnlEditarVARSet}" id="pnlEditarVARSet"
                                            style="style&#x9;border-top: solid navy 1px; border-bottom: solid navy 1px; height: 166px; position: relative; width: 100%; -rave-layout: grid" visible="false">
                                            <webuijsf:staticText id="staticText19"
                                                style="border-bottom: solid navy 3px; background-color: rgb(160, 194, 214); color: navy; font-size: 18px; font-variant: small-caps; font-weight: bolder; height: 28px; left: 0px; top: 0px; position: absolute; text-align: left; width: 100%" text="Datos Modificables para la Variable tipo Set"/>
                                            <webuijsf:label id="label15" style="left: 0px; top: 48px; position: absolute; text-align: center; width: 100%" text="Definición en Lenguaje TOL"/>
                                            <webuijsf:textField binding="#{Explorador.txtVARSet}" columns="94" id="txtVARSet" style="height: 32px; left: 48px; top: 72px; position: absolute; width: 90%"/>
                                            <webuijsf:panelLayout id="layoutPanel10" panelLayout="flow" style="border-width: 1px; border-style: dashed; background-color: rgb(130, 154, 179); height: 32px; left: 20%; top: 120px; position: absolute; text-align: center; width: 60%">
                                                <webuijsf:button actionExpression="#{Explorador.btnModificarVARSet_action}" id="btnModificarVARSet"
                                                    style="height: 24px; margin-right: 8px; margin-top: 4px; width: 120px" text="Modificar Vor"/>
                                                <webuijsf:button actionExpression="#{Explorador.btnCancelarSet_action}" id="btnCancelarSet"
                                                    style="height: 24px; margin-left: 8px; margin-top: 4px; width: 120px" text="Cancelar"/>
                                            </webuijsf:panelLayout>
                                        </webuijsf:panelLayout>
                                    </h:panelGrid>
                                </webuijsf:tab>
                                <webuijsf:tab id="tab3" text="Código Fuente">
                                    <h:panelGrid id="gridPanel2" style="width: 100%">
                                        <webuijsf:staticText id="staticText11"
                                            style="background-color: rgb(155, 230, 251); color: navy; font-size: 14px; font-variant: small-caps; font-weight: bolder; height: 21px; text-align: left; width: 100%" text="Panel de Visor de Código Fuente"/>
                                        <pre class="brush: java; highlight:[2, 5, 10]; toolbar: false;">
                                            //Se deben importar las clases a usar del paquete de TOLJava
                                            import com.hcarralero.toljava.TOLMachine;
                                            ...
                                            //Se crea una instancia de TOLMachine, la cual nos sirve para
                                            //acceder al Intérprete de TOL y poder consultarlo sobre las
                                            //variables que se han definido, "tm" es la instancia creada.
                                            TOLMachine tm= new TOLMachine();
                                            ...
                                            //Usamos un arreglo para almacenar las variables que se definen
                                            //en el intérprete. Este arreglo puede ser llenado con las variables
                                            //que nos retorna el intérprete. En la Aplicación DEMO, la Tabla de
                                            //Variables está asociada a la variable "vars" siguiente:
                                            private TOLVariable[] vars = null;
                                            ...
                                            //Hay varios métodos de TOLMachine que podemos usar para obtener las
                                            //variables definidas en el intérprete. Cada método se corresponde con
                                            //un tipo de dato de TOL. El uso de estos métodos se ilustra en los
                                            //ejemplos de código siguientes, tomados de la Aplicación DEMO
                                            ...
                                            //1er Ejemplo: Obtención de Variables Tipo Text
                                            vars= tm.getTextVariables();
                                            ...
                                            //2do Ejemplo: Obtención de Variables Tipo Real
                                            vars= tm.getRealVariables();
                                            ...
                                            //3er Ejemplo: Obtención de Variables Tipo Date
                                            vars= tm.getDateVariables();
                                            ...
                                            //4to Ejemplo: Obtención de Variables Tipo Complex
                                            vars= tm.getCpmplexVariables();
                                            ...
                                            //En la Aplicación DEMO se ejecuta uno de los ejemplos anteriores cuando
                                            //se hace click en el botón "Ver Variables", según la opción escogida en
                                            //los RadioButtoms.
                                            ...
                                            //Como el valor que puede tener una variable depende del tipo de dato TOL
                                            //al que se asocie, entonces es posible saber el tipo por la clase a la
                                            //que pertenece el valor que contiene.
                                            ...
                                            //"var" es una Variable
                                            TOLVariable var= ...;
                                            //"to" es un objeto que almacena el valor actual de la variable.
                                            TOLObject to= var.getValue();
                                            ...
                                            //Preguntando por la clase de "to" se puede saber el tipo de dato
                                            //Determinar si "to" es un Text de TOL
                                            if (to.getClass()==TOLText.class) {
                                                //"to" representa un valor de tipo "Text" en TOL.
                                                //Con esta confirmación podremos hacer un "casting seguro" al tipo
                                                //de dato correcto.
                                                TOLText tt= (TOLText)to;
                                                //Ahora podemos acceder a los campos correctos para un objeto de
                                                //tipo TOLText.
                                                String text= tt.getText();
                                                ...
                                            } else {
                                                //"to" NO representa un valor de tipo "Text" en TOL.
                                                //Ahora deberíamos averiguar por otro tipo de dato para "trabajar"
                                                //correctamente con "to".
                                                ...
                                            }
                                            ...
                                            //Determinar si "to" es un Real de TOL
                                            if (to.getClass()==TOLReal.class) {
                                                //"to" representa un valor de tipo "Real" en TOL.
                                                //Con esta confirmación podremos hacer un "casting seguro" al tipo
                                                //de dato correcto.
                                                TOLReal tr= (TOLReal)to;
                                                //Ahora podemos acceder a los campos correctos para un objeto de
                                                //ipo TOLReal.
                                                double value= tr.getValue();
                                                ...
                                            } else {
                                                //"to" NO representa un valor de tipo "Text" en TOL.
                                                //Ahora deberíamos averiguar por otro tipo de dato para "trabajar"
                                                //correctamente con "to".
                                                ...
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
