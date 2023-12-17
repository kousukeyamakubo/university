package lifegame;

import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import javax.swing.JButton;
import javax.swing.SwingUtilities;

public class Button implements ActionListener {
	private int buttonId;
	private BoardView boardView;
	private BoardModel boardModel;
	private JButton undoButton;
	
	public Button(int buttonId, BoardView boardView,BoardModel boardModel,JButton undoButton) {
		this.buttonId = buttonId;
		this.boardView = boardView;
		this.boardModel = boardModel;
		this.undoButton = undoButton;
	}
	
	public void actionPerformed(ActionEvent e) {
        switch (buttonId) {
            case 1:
            	boardModel.next();
            	undoButton.setEnabled(true);
                break;
            case 2:
            	if(boardModel.isUndoable()) {
            		boardModel.undo();
            		if(!boardModel.isUndoable()) {
                		undoButton.setEnabled(false);
                	}
            	}
                break;
            case 3:
            	SwingUtilities.invokeLater(new Main());
            	break;
        }
    }
}
