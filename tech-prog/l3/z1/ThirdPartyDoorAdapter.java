package eu.jpereira.trainings.designpatterns.structural.adapter.thirdparty;

import eu.jpereira.trainings.designpatterns.structural.adapter.exceptions.CodeMismatchException;
import eu.jpereira.trainings.designpatterns.structural.adapter.exceptions.IncorrectDoorCodeException;
import eu.jpereira.trainings.designpatterns.structural.adapter.model.Door;

public class ThirdPartyDoorAdapter extends ThirdPartyDoor implements Door  {
    private static final long serialVersionUID = 1L;

    public void open(String code) throws IncorrectDoorCodeException {
        try {
            super.unlock(code);
        } catch (Exception e) {
            throw new IncorrectDoorCodeException();
        }
    }


    public void close() {
         super.lock();
    }


    public boolean isOpen() {
        return super.getLockStatus() == LockStatus.UNLOCKED;
    }

    public void changeCode(String oldCode, String newCode, String newCodeConfirmation)
            throws IncorrectDoorCodeException, CodeMismatchException {
        if (!newCode.equals(newCodeConfirmation)) {
            throw new CodeMismatchException();
        }

        try {
            super.unlock(oldCode);   
            super.setNewLockCode(newCode);
            super.lock();
        } catch (Exception e) {
            throw new IncorrectDoorCodeException();
        }
    }

    public boolean testCode(String code) {
        try {
            super.unlock(code);
            super.lock();
            return true;
        } catch (Exception e) {
            return false;
        }
    }
}
