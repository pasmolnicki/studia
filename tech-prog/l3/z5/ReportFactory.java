package eu.jpereira.trainings.designpatterns.creational.abstractfactory;

import eu.jpereira.trainings.designpatterns.creational.abstractfactory.json.JSONReportBody;
import eu.jpereira.trainings.designpatterns.creational.abstractfactory.json.JSONReportFooter;
import eu.jpereira.trainings.designpatterns.creational.abstractfactory.json.JSONReportHeader;
import eu.jpereira.trainings.designpatterns.creational.abstractfactory.xml.XMLReportBody;
import eu.jpereira.trainings.designpatterns.creational.abstractfactory.xml.XMLReportFooter;
import eu.jpereira.trainings.designpatterns.creational.abstractfactory.xml.XMLReportHeader;


final public class ReportFactory implements AbstractReportFactory {

    @Override
    public ReportBody createReportBody(String reportType) {
        switch (reportType) {
            case "JSON":
                return new JSONReportBody();
            case "XML":
                return new XMLReportBody();
            default:
                return null;
        }
    }

    @Override
    public ReportFooter createReportFooter(String reportType) {
        switch (reportType) {
            case "JSON":
                return new JSONReportFooter();
            case "XML":
                return new XMLReportFooter();
            default:
                return null;
        }
    }

    @Override
    public ReportHeader createReportHeader(String reportType) {
        switch (reportType) {
            case "JSON":
                return new JSONReportHeader();
            case "XML":
                return new XMLReportHeader();
            default:
                return null;
        }
    }
}
