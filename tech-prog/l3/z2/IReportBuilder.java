package eu.jpereira.trainings.designpatterns.creational.builder;

import eu.jpereira.trainings.designpatterns.creational.builder.model.ReportBody;
import eu.jpereira.trainings.designpatterns.creational.builder.model.SaleEntry;

public interface IReportBuilder {
    public void buildReport(SaleEntry saleEntry);
    public ReportBody getReportBody();
}
