//
//  --MODULENAME--Wireframe.swift
//
//  Created by VIPER GENERATOR on --TODAY--.
//

import UIKit

class --MODULENAME--Wireframe {
     weak var view: UIViewController!
}

extension --MODULENAME--Wireframe: --MODULENAME--WireframeInput {
    
    var storyboardName: String {return "--MODULENAME--"}
    
    func getMainView() -> UIViewController {
        let service = --MODULENAME--Service()
        let interactor = --MODULENAME--Interactor(service: service)
        let presenter = --MODULENAME--Presenter()
        let viewController = viewControllerFromStoryboard(of: --MODULENAME--ViewController.self)
        
        viewController.presenter = presenter
        interactor.output = presenter
        presenter.interactor = interactor
        presenter.wireframe = self
        presenter.view = viewController
        
        self.view = viewController
        return viewController
    }
}
