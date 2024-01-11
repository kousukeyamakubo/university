package lifegame;

import javax.swing.SwingUtilities;

import javax.swing.JFrame;
import javax.swing.JPanel;
import java.awt.Dimension;
import java.awt.BorderLayout;
import javax.swing.JButton;
import java.awt.FlowLayout;

public class Main implements Runnable {
	private BoardModel model;
	JButton Undo = new JButton("Undo");
	
	public static void main(String[] args) {
        SwingUtilities.invokeLater(new Main());
    }
	
	public void run() {
		int M=12;
		int N=12;
		if(M<=0||M>282||N<=0||N>123) {
			System.out.println("Error");
			return;
		}
		model = new BoardModel(M,N,Undo);
		model.addListener(new BoardView(model));
		JFrame frame = new JFrame();
		frame.setDefaultCloseOperation(JFrame.DISPOSE_ON_CLOSE);
		JPanel base = new JPanel();
		frame.setContentPane(base);
		base.setLayout(new BorderLayout());
		BoardView view = new BoardView(model);
		base.add(view, BorderLayout.CENTER);
		JPanel buttonPanel = new JPanel();
		base.add(buttonPanel, BorderLayout.SOUTH);
		buttonPanel.setLayout(new FlowLayout());
		JButton Next = new JButton("Next");
		Undo.setEnabled(false);
		JButton Newgame = new JButton("New Game");
		Button buttonListener1 = new Button(1,model,Undo);
		Next.addActionListener(buttonListener1);
		Button buttonListener2 = new Button(2,model,Undo);
		Undo.addActionListener(buttonListener2);
		Button buttonListener3 = new Button(3,model,Undo);
		Newgame.addActionListener(buttonListener3);
		buttonPanel.add(Next);
		buttonPanel.add(Undo);
		buttonPanel.add(Newgame);
		base.setPreferredSize(new Dimension(400, 300));
		frame.setMinimumSize(new Dimension(300, 200));
		frame.setTitle("Lifegame");
		frame.pack();
		frame.setVisible(true);
	}
}
