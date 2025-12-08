package eu.jpereira.trainings.designpatterns.creational.factorymethod;

final public class ReportFactory {
    static public Report createReport(final String type) {
        switch (type) {
            case "JSON":
                return new JSONReport();
            case "XML":
                return new XMLReport();
            case "HTML":
                return new HTMLReport();
            case "PDF":
                return new PDFReport();
            default:
                return null;
        }
    }
}
