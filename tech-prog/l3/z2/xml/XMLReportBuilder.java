package eu.jpereira.trainings.designpatterns.creational.builder.xml;

import eu.jpereira.trainings.designpatterns.creational.builder.IReportBuilder;
import eu.jpereira.trainings.designpatterns.creational.builder.model.ReportBody;
import eu.jpereira.trainings.designpatterns.creational.builder.model.SaleEntry;
import eu.jpereira.trainings.designpatterns.creational.builder.model.SoldItem;
import java.util.Iterator;

public class XMLReportBuilder implements IReportBuilder {
    private XMLReportBody reportBody;
    
    public void buildReport(SaleEntry saleEntry) {
        reportBody = new XMLReportBody();
        reportBody.putContent("<sale><customer><name>");
        reportBody.putContent(saleEntry.getCustomer().getName());
        reportBody.putContent("</name><phone>");
        reportBody.putContent(saleEntry.getCustomer().getPhone());
        reportBody.putContent("</phone></customer>");
        
        reportBody.putContent("<items>");
        
        Iterator<SoldItem> it = saleEntry.getSoldItems().iterator();
        while ( it.hasNext() ) {
            SoldItem soldEntry= it.next();
            reportBody.putContent("<item><name>");
            reportBody.putContent(soldEntry.getName());
            reportBody.putContent("</name><quantity>");
            reportBody.putContent(String.valueOf(soldEntry.getQuantity()));
            reportBody.putContent("</quantity><price>");
            reportBody.putContent(String.valueOf(soldEntry.getUnitPrice()));
            reportBody.putContent("</price></item>");
        }
        
        reportBody.putContent("</items></sale>");
    }

    public ReportBody getReportBody() {
        return this.reportBody;
    }
    
}
