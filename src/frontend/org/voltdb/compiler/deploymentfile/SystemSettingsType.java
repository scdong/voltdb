//
// This file was generated by the JavaTM Architecture for XML Binding(JAXB) Reference Implementation, vJAXB 2.1.10 in JDK 6 
// See <a href="http://java.sun.com/xml/jaxb">http://java.sun.com/xml/jaxb</a> 
// Any modifications to this file will be lost upon recompilation of the source schema. 
// Generated on: 2011.09.26 at 12:40:31 PM EDT 
//


package org.voltdb.compiler.deploymentfile;

import javax.xml.bind.annotation.XmlAccessType;
import javax.xml.bind.annotation.XmlAccessorType;
import javax.xml.bind.annotation.XmlAttribute;
import javax.xml.bind.annotation.XmlType;


/**
 * <p>Java class for systemSettingsType complex type.
 * 
 * <p>The following schema fragment specifies the expected content contained within this class.
 * 
 * <pre>
 * &lt;complexType name="systemSettingsType">
 *   &lt;complexContent>
 *     &lt;restriction base="{http://www.w3.org/2001/XMLSchema}anyType">
 *       &lt;all>
 *         &lt;element name="temptables" minOccurs="0">
 *           &lt;complexType>
 *             &lt;complexContent>
 *               &lt;restriction base="{http://www.w3.org/2001/XMLSchema}anyType">
 *                 &lt;attribute name="maxsize" type="{}memorySizeType" default="100" />
 *               &lt;/restriction>
 *             &lt;/complexContent>
 *           &lt;/complexType>
 *         &lt;/element>
 *         &lt;element name="snapshot" minOccurs="0">
 *           &lt;complexType>
 *             &lt;complexContent>
 *               &lt;restriction base="{http://www.w3.org/2001/XMLSchema}anyType">
 *                 &lt;attribute name="priority" type="{}snapshotPriorityType" default="6" />
 *               &lt;/restriction>
 *             &lt;/complexContent>
 *           &lt;/complexType>
 *         &lt;/element>
 *       &lt;/all>
 *     &lt;/restriction>
 *   &lt;/complexContent>
 * &lt;/complexType>
 * </pre>
 * 
 * 
 */
@XmlAccessorType(XmlAccessType.FIELD)
@XmlType(name = "systemSettingsType", propOrder = {

})
public class SystemSettingsType {

    protected SystemSettingsType.Temptables temptables;
    protected SystemSettingsType.Snapshot snapshot;

    /**
     * Gets the value of the temptables property.
     * 
     * @return
     *     possible object is
     *     {@link SystemSettingsType.Temptables }
     *     
     */
    public SystemSettingsType.Temptables getTemptables() {
        return temptables;
    }

    /**
     * Sets the value of the temptables property.
     * 
     * @param value
     *     allowed object is
     *     {@link SystemSettingsType.Temptables }
     *     
     */
    public void setTemptables(SystemSettingsType.Temptables value) {
        this.temptables = value;
    }

    /**
     * Gets the value of the snapshot property.
     * 
     * @return
     *     possible object is
     *     {@link SystemSettingsType.Snapshot }
     *     
     */
    public SystemSettingsType.Snapshot getSnapshot() {
        return snapshot;
    }

    /**
     * Sets the value of the snapshot property.
     * 
     * @param value
     *     allowed object is
     *     {@link SystemSettingsType.Snapshot }
     *     
     */
    public void setSnapshot(SystemSettingsType.Snapshot value) {
        this.snapshot = value;
    }


    /**
     * <p>Java class for anonymous complex type.
     * 
     * <p>The following schema fragment specifies the expected content contained within this class.
     * 
     * <pre>
     * &lt;complexType>
     *   &lt;complexContent>
     *     &lt;restriction base="{http://www.w3.org/2001/XMLSchema}anyType">
     *       &lt;attribute name="priority" type="{}snapshotPriorityType" default="6" />
     *     &lt;/restriction>
     *   &lt;/complexContent>
     * &lt;/complexType>
     * </pre>
     * 
     * 
     */
    @XmlAccessorType(XmlAccessType.FIELD)
    @XmlType(name = "")
    public static class Snapshot {

        @XmlAttribute
        protected Integer priority;

        /**
         * Gets the value of the priority property.
         * 
         * @return
         *     possible object is
         *     {@link Integer }
         *     
         */
        public int getPriority() {
            if (priority == null) {
                return  6;
            } else {
                return priority;
            }
        }

        /**
         * Sets the value of the priority property.
         * 
         * @param value
         *     allowed object is
         *     {@link Integer }
         *     
         */
        public void setPriority(Integer value) {
            this.priority = value;
        }

    }


    /**
     * <p>Java class for anonymous complex type.
     * 
     * <p>The following schema fragment specifies the expected content contained within this class.
     * 
     * <pre>
     * &lt;complexType>
     *   &lt;complexContent>
     *     &lt;restriction base="{http://www.w3.org/2001/XMLSchema}anyType">
     *       &lt;attribute name="maxsize" type="{}memorySizeType" default="100" />
     *     &lt;/restriction>
     *   &lt;/complexContent>
     * &lt;/complexType>
     * </pre>
     * 
     * 
     */
    @XmlAccessorType(XmlAccessType.FIELD)
    @XmlType(name = "")
    public static class Temptables {

        @XmlAttribute
        protected Integer maxsize;

        /**
         * Gets the value of the maxsize property.
         * 
         * @return
         *     possible object is
         *     {@link Integer }
         *     
         */
        public int getMaxsize() {
            if (maxsize == null) {
                return  100;
            } else {
                return maxsize;
            }
        }

        /**
         * Sets the value of the maxsize property.
         * 
         * @param value
         *     allowed object is
         *     {@link Integer }
         *     
         */
        public void setMaxsize(Integer value) {
            this.maxsize = value;
        }

    }

}
