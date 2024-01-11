package lifegame;

import java.util.ArrayList;
import javax.swing.JButton;

public class BoardModel{
	private int cols;
	private int rows;
	private int counter=0;
	private boolean[][] cells;
	private boolean[][] copiedcells;
	private ArrayList<BoardListener> listeners;
	private ArrayList<boolean[][]> History;
	private JButton undoButton;
	
	public BoardModel(int c,int r,JButton undoButton) {
		cols=c;
		rows=r;
		cells=new boolean[rows][cols];
		copiedcells = new boolean[rows][cols];
		listeners = new ArrayList<BoardListener>();
		History = new ArrayList<boolean[][]>();
		History.add(new boolean[rows][cols]);
		this.undoButton = undoButton;
	}
	
	public int getCols() {
		return cols;
	}
	
	public int getRows() {
		return rows;
	}
	
	public void changeCellState(int x,int y) {
		undoButton.setEnabled(true);
		if(cells[y][x]==true) {
			cells[y][x]=false;
		}else {
			cells[y][x]=true;
		}
		boolean[][] currentBoard = new boolean[rows][cols];
        for (int i = 0; i < rows; i++) {
            System.arraycopy(cells[i], 0, currentBoard[i], 0, cols);
        }
        if(counter!=32) {
        	counter++;
        	History.add(currentBoard);
        }else {
        	History.remove(0);
        	History.add(currentBoard);
        }
		this.fireUpdate();
	}
	
	public void addListener(BoardListener listener) {
		listeners.add(listener);
	}
	
	private void fireUpdate() {
		for(BoardListener listener:listeners) {
			listener.updated(this);
		}
	}
	
	public void next() {
		for(int i = 0; i < rows; i++) {
			for(int j = 0; j < cols; j++) {
				copiedcells[i][j] = cells[i][j];
			}
		}
		for(int i=0;i<cols;i++) {
			for(int j=0;j<rows;j++) {
				int num = this.check(i,j);
				if(cells[j][i]==true) {
					if(num == 2|| num == 3) {
						copiedcells[j][i] = true;
					}else {
						copiedcells[j][i] = false;
					}
				}
				if(cells[j][i]==false) {
					if(num == 3) {
						copiedcells[j][i] = true;
					}else {
						copiedcells[j][i] = false;
					}
				}
			}
		}
		
		boolean[][] currentBoard = new boolean[rows][cols];
        for (int i = 0; i < rows; i++) {
            System.arraycopy(cells[i], 0, currentBoard[i], 0, cols);
        }

        if(counter!=32) {
        	counter++;
        	History.add(currentBoard);
        }else {
        	History.remove(0);
        	History.add(currentBoard);
        }
		
		for(int i = 0; i < rows; i++) {
			for(int j = 0; j < cols; j++) {
				cells[i][j] = copiedcells[i][j];
			}
		}
		this.fireUpdate();
	}
	
	public int check(int x,int y) {
		int count = 0;
		if(x-1>=0 && y-1 >= 0 && cells[y-1][x-1] == true) {
			count++;
		}
		if(x-1>=0 && cells[y][x-1] == true) {
			count++;
		}
		if(x-1>=0 && y+1 <= rows-1 && cells[y+1][x-1] == true) {
			count++;
		}
		if(y-1 >= 0 && cells[y-1][x] == true) {
			count++;
		}
		if(y+1<=rows-1 && cells[y+1][x] == true) {
			count++;
		}
		if(x+1<=cols-1 && y+1 <= rows-1 && cells[y+1][x+1] == true) {
			count++;
		}
		if(x+1<=cols-1 && cells[y][x+1] == true) {
			count++;
		}
		if(x+1<=cols-1 && y-1 >= 0 && cells[y-1][x+1] == true) {
			count++;
		}
		return count;
	}
	public void undo() {
		History.remove(counter);
		counter--;
        cells= History.get(counter);
        this.fireUpdate();
	}
	
	public boolean isUndoable() {
		if(counter>0) {
			return true;
		}else {
			return false;
		}
	}
	
	public boolean isAlive(int x,int y) {
		if(x>=0 && x<cols && y>=0 && y<rows) {
			return cells[y][x];
		}
		return false;
	}
}
