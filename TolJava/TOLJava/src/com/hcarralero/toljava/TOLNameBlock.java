package com.hcarralero.toljava;

/**
 * Representa a un NameBlock de TOL.
 *
 * @author Humberto Andrés Carralero López (hcarralero@gmail.com)
 * @version 1.0
 * @see com.hcarralero.toljava.TOLMachineException
 */
public class TOLNameBlock {

    private String name;

    /**
     * Obtiene el nombre del NameBlock
     *
     * @return el nombre del NameBlock
     */
    public String getName() {
        return name;
    }

    /**
     * Construye un NameBlock
     *
     * @param name nombre del NameBlock
     */
    public TOLNameBlock(String name) {
        this.name = name;
    }

}
