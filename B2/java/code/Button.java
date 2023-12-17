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
        // ボタンの識別子に基づいて処理を実行
        switch (buttonId) {
            case 1:
            	//nextが押された時の処理
            	boardModel.next();
            	undoButton.setEnabled(true);
            	boardView.repaint();
                break;
            case 2:
            	//undoが押された時の処理
            	if(boardModel.isUndoable()) {
            		boardModel.undo();
            		boardView.repaint();
            		if(!boardModel.isUndoable()) {
                		undoButton.setEnabled(false);
                	}
            		boardView.repaint();
            	}
                break;
            case 3:
            	SwingUtilities.invokeLater(new Main());
            	break;
        }
    }
}
