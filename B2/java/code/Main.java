package lifegame;

import javax.swing.SwingUtilities;

import javax.swing.JFrame;
import javax.swing.JPanel;
import java.awt.Dimension;
import java.awt.BorderLayout;
import javax.swing.JButton;
import java.awt.FlowLayout;

import java.awt.event.ActionListener;

public class Main implements Runnable {
	private BoardModel model;
	private BoardView view;
	JButton Undo = new JButton("Undo");
	
	public static void main(String[] args) {
        SwingUtilities.invokeLater(new Main());
    }
	
	public void run() {
		model = new BoardModel(14,12,Undo);
		model.addListener(new BoardView(model));
		// BoardModel の作成と changeCellState の呼び出しを行う処理をここで実行。
		// next と undo の呼び出しを取り除き、「グライダー」が設置された状態としておく。
		// ウィンドウを作成する
		
		JFrame frame = new JFrame();
		frame.setDefaultCloseOperation(JFrame.DISPOSE_ON_CLOSE);
		// ウィンドウ内部を占有する「ベース」パネルを作成する
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
		
		// ボタン1用のListenerを作成
		Button buttonListener1 = new Button(1,view,model,Undo);
		Next.addActionListener(buttonListener1);

		// ボタン2用のListenerを作成
		Button buttonListener2 = new Button(2,view,model,Undo);
		Undo.addActionListener(buttonListener2);

		// ボタン3用のListenerを作成
		Button buttonListener3 = new Button(3,view,model,Undo);
		Newgame.addActionListener(buttonListener3);
		
		
		buttonPanel.add(Next);
		buttonPanel.add(Undo);
		buttonPanel.add(Newgame);
		
		base.setPreferredSize(new Dimension(400, 300)); // 希望サイズの指定
		frame.setMinimumSize(new Dimension(300, 200)); // 最小サイズの指定
		frame.setTitle("Lifegame");
		
		frame.pack(); // ウィンドウに乗せたものの配置を確定する
		frame.setVisible(true); // ウィンドウを表示する
	}
}
