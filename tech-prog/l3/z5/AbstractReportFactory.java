package eu.jpereira.trainings.designpatterns.creational.abstractfactory;

public interface AbstractReportFactory {
    ReportBody createReportBody(String reportType);
    ReportFooter createReportFooter(String reportType);
    ReportHeader createReportHeader(String reportType);
}
