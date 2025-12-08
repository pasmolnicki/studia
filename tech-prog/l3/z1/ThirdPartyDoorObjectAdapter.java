package eu.jpereira.trainings.designpatterns.structural.adapter.thirdparty;

import eu.jpereira.trainings.designpatterns.structural.adapter.model.Door;
import eu.jpereira.trainings.designpatterns.structural.adapter.thirdparty.exceptions.*;
import eu.jpereira.trainings.designpatterns.structural.adapter.exceptions.*;


public class ThirdPartyDoorObjectAdapter implements Door {
    public static final String DEFAULT_CODE = "AAAAHHHH";
    private ThirdPartyDoor thirdPartyDoor = new ThirdPartyDoor();

    public void open(String code) throws IncorrectDoorCodeException {
        try {
            thirdPartyDoor.unlock(code);
        } catch (Exception e) {
            throw new IncorrectDoorCodeException();
        }
    }

    public void close() {
        thirdPartyDoor.lock();
    }

    public boolean isOpen() {
        return thirdPartyDoor.getLockStatus() == ThirdPartyDoor.LockStatus.UNLOCKED;
    }

    public void changeCode(String oldCode, String newCode, String newCodeConfirmation)
            throws IncorrectDoorCodeException,
            CodeMismatchException {
        if (!newCode.equals(newCodeConfirmation)) {
            throw new CodeMismatchException();
        }

        try {
            thirdPartyDoor.unlock(oldCode);
            thirdPartyDoor.setNewLockCode(newCode);
            thirdPartyDoor.lock();
        } catch (Exception e) {
            throw new IncorrectDoorCodeException();
        }
    }

    public boolean testCode(String code) {
        try {
            thirdPartyDoor.unlock(code);
            thirdPartyDoor.lock();
            return true;
        } catch (Exception e) {
            return false;
        }
    }
}
