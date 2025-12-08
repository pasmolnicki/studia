package eu.jpereira.trainings.designpatterns.creational.builder;

import eu.jpereira.trainings.designpatterns.creational.builder.model.ReportBody;
import eu.jpereira.trainings.designpatterns.creational.builder.model.SaleEntry;

public interface IBuilder {
    void construct(String type, SaleEntry saleEntry);
    ReportBody getReportBody();
}
