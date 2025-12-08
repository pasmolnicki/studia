package eu.jpereira.trainings.designpatterns.creational.builder;

import eu.jpereira.trainings.designpatterns.creational.builder.html.HTMLReportBuilder;
import eu.jpereira.trainings.designpatterns.creational.builder.json.JSONReportBuilder;
import eu.jpereira.trainings.designpatterns.creational.builder.model.ReportBody;
import eu.jpereira.trainings.designpatterns.creational.builder.model.SaleEntry;
import eu.jpereira.trainings.designpatterns.creational.builder.xml.XMLReportBuilder;

public class DefaultBuilder implements IBuilder {
    private ReportBody reportBody = null;

    public void construct(String type, SaleEntry saleEntry) {
        IReportBuilder builder = null;
        this.reportBody = null;

        if (type.equals("JSON")) {
            builder = new JSONReportBuilder();
        } else if (type.equals("XML")) {
            builder = new XMLReportBuilder();
        } else if (type.equals("HTML")) {
            builder = new HTMLReportBuilder();
        }

        if (builder != null) {
            builder.buildReport(saleEntry);
            this.reportBody = builder.getReportBody();
        }
    }

    public ReportBody getReportBody() {
        return this.reportBody;
    }
}